#include "app_cli.h"

app_cli_status_e cli__hello(app_cli__argument_t argument, sl_string_t user_input_minus_command_name,
                            app_cli__print_string_function cli_output);

app_cli_status_e cli__task_list(app_cli__argument_t argument, sl_string_t user_input_minus_command_name,
                                app_cli__print_string_function cli_output);

app_cli_status_e a_cli_handler_for_task(app_cli__argument_t argument, sl_string_t user_input_minus_command_name,
                                        app_cli__print_string_function cli_output);