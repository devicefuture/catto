void catto_addContextStandardCommands(catto_Context* context) {
    // Control flow
    catto_addCommand(context, "goto", &catto_command_goto);
    catto_addCommand(context, "gosub", &catto_command_gosub);
    catto_addCommand(context, "return", &catto_command_return);
    catto_addCommand(context, "if", &catto_command_if);
    catto_addCommand(context, "else", &catto_command_else);
    catto_addCommand(context, "end", &catto_command_end);
    catto_addCommand(context, "for", &catto_command_for);
    catto_addCommand(context, "next", &catto_command_next);
    catto_addCommand(context, "repeat", &catto_command_repeat);
    catto_addCommand(context, "while", &catto_command_while);
    catto_addCommand(context, "until", &catto_command_until);
    catto_addCommand(context, "loop", &catto_command_loop);
    catto_addCommand(context, "break", &catto_command_break);
    catto_addCommand(context, "continue", &catto_command_continue);
    catto_addCommand(context, "stop", &catto_command_stop);

    // I/O
    catto_addCommand(context, "print", &catto_command_print);

    // Lists
    catto_addCommand(context, "dim", &catto_command_dim);
    catto_addCommand(context, "push", &catto_command_push);
    catto_addCommand(context, "pop", &catto_command_pop);
    catto_addCommand(context, "insert", &catto_command_insert);
    catto_addCommand(context, "remove", &catto_command_remove);

    // Functions
    catto_addFunction(context, "round", &catto_function_round);
    catto_addFunction(context, "floor", &catto_function_floor);
    catto_addFunction(context, "ceil", &catto_function_ceil);
    catto_addFunction(context, "abs", &catto_function_abs);
    catto_addFunction(context, "min", &catto_function_min);
    catto_addFunction(context, "max", &catto_function_max);
    catto_addFunction(context, "lower", &catto_function_lower);
    catto_addFunction(context, "upper", &catto_function_upper);
}