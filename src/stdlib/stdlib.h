void catto_addContextStandardCommands(catto_Context* context) {
    // Control flow

    catto_addCommand(context, "goto", &catto_command_goto);
    catto_addCommand(context, "gosub", &catto_command_gosub);
    catto_addCommand(context, "def", &catto_command_def);
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

    #ifndef CATTO_CUSTOM_PRINT_COMMAND
        catto_addCommand(context, "print", &catto_command_print);
    #endif

    catto_addCommand(context, "scrawl", &catto_command_scrawl);
    catto_addCommand(context, "noscrawl", &catto_command_noscrawl);

    // Lists

    catto_addCommand(context, "dim", &catto_command_dim);
    catto_addCommand(context, "push", &catto_command_push);
    catto_addCommand(context, "pop", &catto_command_pop);
    catto_addCommand(context, "insert", &catto_command_insert);
    catto_addCommand(context, "remove", &catto_command_remove);

    // Functions

    catto_addCommand(context, "deg", &catto_command_deg);
    catto_addCommand(context, "rad", &catto_command_rad);
    catto_addCommand(context, "gon", &catto_command_gon);
    catto_addCommand(context, "turn", &catto_command_turn);

    catto_addFunction(context, "sin", &catto_function_sin);
    catto_addFunction(context, "cos", &catto_function_cos);
    catto_addFunction(context, "tan", &catto_function_tan);
    catto_addFunction(context, "asin", &catto_function_asin);
    catto_addFunction(context, "acos", &catto_function_acos);
    catto_addFunction(context, "atan", &catto_function_atan);
    catto_addFunction(context, "log", &catto_function_log);
    catto_addFunction(context, "ln", &catto_function_ln);
    catto_addFunction(context, "sqrt", &catto_function_sqrt);
    catto_addFunction(context, "round", &catto_function_round);
    catto_addFunction(context, "floor", &catto_function_floor);
    catto_addFunction(context, "ceil", &catto_function_ceil);
    catto_addFunction(context, "abs", &catto_function_abs);
    catto_addFunction(context, "min", &catto_function_min);
    catto_addFunction(context, "max", &catto_function_max);
    catto_addFunction(context, "asc", &catto_function_asc);
    catto_addFunction(context, "chr", &catto_function_chr);
    catto_addFunction(context, "bin", &catto_function_bin);
    catto_addFunction(context, "oct", &catto_function_oct);
    catto_addFunction(context, "hex", &catto_function_hex);
    catto_addFunction(context, "len", &catto_function_len);
    catto_addFunction(context, "last", &catto_function_last);
    catto_addFunction(context, "split", &catto_function_split);
    catto_addFunction(context, "join", &catto_function_join);
    catto_addFunction(context, "find", &catto_function_find);
    catto_addFunction(context, "lower", &catto_function_lower);
    catto_addFunction(context, "upper", &catto_function_upper);
    catto_addFunction(context, "left", &catto_function_left);
    catto_addFunction(context, "right", &catto_function_right);
    catto_addFunction(context, "mid", &catto_function_mid);
    catto_addFunction(context, "trim", &catto_function_trim);
    catto_addFunction(context, "ltrim", &catto_function_ltrim);
    catto_addFunction(context, "rtrim", &catto_function_rtrim);
    catto_addFunction(context, "repeat", &catto_function_repeat);

    // Constants

    catto_setVariable(context, "true", catto_asTypedNumber(1));
    catto_setVariable(context, "false", catto_asTypedNumber(0));
    catto_setVariable(context, "pi", catto_asTypedNumber(CATTO_PI));
    catto_setVariable(context, "e", catto_asTypedNumber(CATTO_E));
    catto_setVariable(context, "phi", catto_asTypedNumber(CATTO_PHI));
}