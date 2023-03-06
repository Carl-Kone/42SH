import test_builtins_echo
import test_builtins_true_false
import test_command_lists
import test_single_quote
import test_if_commands
import test_comments
import test_double_quotes
import test_redirections
import test_pipelines
import test_pipelines_negation
import test_variable
import test_for_loops
import test_exit
import test_export
import test_builtins_unset
import test_cd

if __name__ == "__main__":
    from tester import summary, test_scripts
    test_scripts()
    summary()
