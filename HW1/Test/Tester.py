import os
import subprocess


def test_failed_prompt(expect, got, order):
    print("=================FAILED TEST", order, "======================")
    print("======Expected", expect, "got", got, "=======")


testCnt = 0
correct = 0
ignored = []
for file in os.listdir():
    if "test" in file:
        testCnt += 1

args = ["clang", "../lab1.c", "-o", "lab1"]
subprocess.run(args)

for i in range(1, testCnt + 1):
    fileStr = "test" + str(i)
    args = ["./lab1", fileStr, "out"]
    subprocess.call(args)
    ans = []
    with open(fileStr) as f:
        startedIn = False
        for line in f:
            if "OUTPUT" in line:
                startedIn = True
                continue
            if startedIn:
                if "no solution\n" not in str(line) and "many solutions\n" not in str(line):
                    ans.append(float(line))
                else:
                    ans.append(line)

    testFailed = False
    with open("out") as f:
        # print("*****TESTING", i, "*****")
        ansCnt = 0
        ansProg = []
        for line in f:
            ansProg.append(line)
            if line != "no solution" and line != "many solutions":
                if abs(float(line) - ans[ansCnt]) > 0.00001:
                    test_failed_prompt(ans[ansCnt], float(line), i)
                    testFailed = True
            else:
                if str(line) + "\n" != ans[ansCnt]:
                    test_failed_prompt(ans[ansCnt], line, i)
                    testFailed = True
            ansCnt += 1
        if ansProg == []:
            print("IGNORED", i) 
            ignored.append(i)
        elif testFailed:
            print("Test ANS", ans)
            print("Prog ANS", ansProg)
    if not testFailed and ansProg != []:
        print("PASSED", i)
        correct += 1

print("FINISHED")
print("Passed", correct, "out of", testCnt - len(ignored), "tests")
print("Ignored tests", ignored)
