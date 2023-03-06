import subprocess as sp
import os

PASSED = []
FAILED = []

def summary():
    for (test_name, ref_proc, student_proc) in FAILED:
        print(test_name, end=" ")
        print('\033[91m' + "FAILED" + '\033[0m')
        print("expected", ref_proc)
        print("got", student_proc)

    n_tests = len(PASSED) + len(FAILED)

    print('\033[92m' + "PASSED: " + str(len(PASSED)) + "/" + str(n_tests) + '\033[0m')

    if len(FAILED) != 0:
        print('\033[91m' + "FAILED: " + str(len(FAILED)) + "/" + str(n_tests) + '\033[0m')


def ref_and_student_files_content_are_identical(ref_files_content,student_files_content):
    for i in range(len(ref_files_content)):
        #print('file' + ' ' + str(i) + 'ref' + ':' + ref_files_content[i])
        #print('file' + ' ' + str(i) + 'student' + ':' + student_files_content[i])
        if ref_files_content[i] != student_files_content[i]:
            return False
    return True


def test_cmd_with_files(test_name, command_str, files):
    ref_proc = sp.run(["bash", "--posix", "-c", command_str], capture_output=True, text=True)
    # ref_files_content[i] = the content from bash --posix of the file named files[i]
    ref_files_content = []
    for file in files:
        file_content = None
        with open(file,"r") as f:
            file_content = f.read()
        ref_files_content.append(file_content)
        os.remove(file)

    student_proc = sp.run(["./42sh", "-c", command_str], capture_output=True, text=True)
    # student_files_content[i] = the content from ./42sh  of the file named files[i]
    student_files_content = []
    for file in files:
        file_content = None
        try:
            with open(file,"r") as f:
                file_content = f.read()
        except OSError:
            print(file + ": File not found")
            FAILED.append((test_name,ref_proc,student_proc))
            return
        student_files_content.append(file_content)
        os.remove(file)

    test_failed = ref_proc.returncode != student_proc.returncode or ref_proc.stdout != student_proc.stdout or (ref_proc.stderr != "" and student_proc.    stderr == "") or (ref_proc.stderr == "" and student_proc.stderr != "") or not ref_and_student_files_content_are_identical(ref_files_content, student_files_content)
    
    if(test_failed):
         FAILED.append((test_name, ref_proc, student_proc))
    else:
         PASSED.append((test_name, ref_proc, student_proc))

def test_cmd(test_name, command_str,files=None):
    if(not files):
        ref_proc = sp.run(["bash", "--posix", "-c", command_str], capture_output=True, text=True)
        student_proc = sp.run(["./42sh", "-c", command_str], capture_output=True, text=True)

        test_failed = ref_proc.returncode != student_proc.returncode or ref_proc.stdout != student_proc.stdout or (ref_proc.stderr != "" and student_proc.stderr == "") or (ref_proc.stderr == "" and student_proc.stderr != "")

        if (test_failed):
            FAILED.append((test_name, ref_proc, student_proc))
        else:
            PASSED.append((test_name, ref_proc, student_proc))
    else:
        test_cmd_with_files(test_name,command_str,files)

def test_scripts():
    scripts = os.listdir("tests/test_scripts")
    for script in scripts:
        if script.endswith(".sh"):
            with open("tests/test_scripts/" + script) as file:
                content = file.read()
            ref_proc = sp.run(["bash", "--posix"], capture_output=True, text=True, input=content)
            student_proc = sp.run(["./42sh"], capture_output=True, text=True, input=content)

            test_failed = ref_proc.returncode != student_proc.returncode or ref_proc.stdout != student_proc.stdout or (ref_proc.stderr != "" and student_proc.stderr == "") or (ref_proc.stderr == "" and student_proc.stderr != "")
            if(test_failed):
                 FAILED.append((script, ref_proc, student_proc))
            else:
                 PASSED.append((script, ref_proc, student_proc))
            
            ref_proc = sp.run(["bash", "--posix", "tests/test_scripts/" +
                script, "1", "2", "3"], capture_output=True, text=True)
            student_proc = sp.run(["./42sh", "tests/test_scripts/" + script, "1", "2", "3"], capture_output=True, text=True)

            test_failed = ref_proc.returncode != student_proc.returncode or ref_proc.stdout != student_proc.stdout or (ref_proc.stderr != "" and student_proc.stderr == "") or (ref_proc.stderr == "" and student_proc.stderr != "")
            if(test_failed):
                 FAILED.append((script, ref_proc, student_proc))
            else:
                 PASSED.append((script, ref_proc, student_proc))
