static const pla_token_info pla_token_infos[] = {
    // 0x0000
    { },

    // 0x0001
    { "init", 3,
        { pla_args_type_16, pla_args_type_16, pla_args_type_16 },
        { "width", "height", "unk" } },

    // 0x0002
    { "render_text", 4,
        { pla_args_type_16, pla_args_type_16, pla_args_type_16, pla_args_type_16 },
        { "x", "y", "color", "line_idx" } },

    // 0x0003
    { "load_ani", 2,
        { pla_args_type_string, pla_args_type_16 },
        { "ani_file", "unk" } },

    // 0x0004
    { "render_ani", 5,
        { pla_args_type_16, pla_args_type_16, pla_args_type_16, pla_args_type_16, pla_args_type_16 },
        { "x", "y", "size", "item_in_ani", "ani_idx" } },

    // 0x0005
    { },

    // 0x0006
    { "opcode_0006", 1,
        { pla_args_type_16 },
        { } },

    // 0x0007
    { "delay", 1,
        { pla_args_type_16 },
        { "d" } },

    // 0x0008
    { },

    // 0x0009
    { "set_swivar", 2,
        { pla_args_type_16, pla_args_type_32 },
        { "swi", "val" } },

    // 0x000a
    { "end", 0,
    { },
        { } },

    // 0x000b
    { "maybe_start_music", 1,
        { pla_args_type_16 },
        { "mus" } },

    // 0x000c
    { "maybe_stop_music", 1,
        { pla_args_type_16 },
        { "mus" } },

    // 0x000d
    { },

    // 0x000e
    { "opcode_000e", 3,
        { pla_args_type_16, pla_args_type_16, pla_args_type_32 },
        { "field1", "field2", "offset" } },

    // 0x000f
    { "opcode_000f", 1,
        { pla_args_type_16 },
        { "field1" } },

    // 0x0010
    { "fade_in", 3,
        { pla_args_type_16, pla_args_type_16, pla_args_type_16 },
        { "color", "time", "ani_idx" } },

    // 0x0011
    { "opcode_0011", 2,
        { pla_args_type_16, pla_args_type_16 },
        { "field1", "field2" } },

    // 0x0012
    { "opcode_0012", 1,
        { pla_args_type_16 },
        { } },

    // 0x0013
    { "maybe_goto_pla", 1,
        { pla_args_type_string },
        { "pla" } },

    // 0x0014
    { },

    // 0x0015
    { },

    // 0x0016
    { "opcode_0016", 2,
        { pla_args_type_16, pla_args_type_32 },
        { } },

    // 0x0017
    { },

    // 0x0018
    { },

    // 0x0019
    { "load_font", 1,
        { pla_args_type_string },
        { "file" } },

    // 0x001a
    { },

    // 0x001b
    { "another_goto", 1,
        { pla_args_type_32 },
        { "pla_offset" } },

    // 0x001c
    { "load_mdi", 1,
        { pla_args_type_string },
        { "file" } },

    // 0x001d
    { "blit_if_needed", 0,
    { },
        { } },

    // 0x001e
    { },

    // 0x001f
    { "reset_switch_var", 1,
        { pla_args_type_16 },
        { "idx" } },

    // 0x0020
    { "set_switch_var", 1,
        { pla_args_type_16 },
        { "idx" } },

    // 0x0021
    { "if_switch", 3,
    { },
        { "switch_idx", "value", "pla_offset" } },

    // 0x0022
    { },

    // 0x0023
    { "load_ptr", 1,
        { pla_args_type_string },
        { "file" } },

    // 0x0024
    { },

    // 0x0025
    { },

    // 0x0026
    { },

    // 0x0027
    { },

    // 0x0028
    { "load_wdw", 2,
        { pla_args_type_string, pla_args_type_16 },
        { "til_file", "unk" } },

    // 0x0029
    { "load_texts", 1,
        { pla_args_type_string },
        { "file" } },

    // 0x002a
    { "prepare_text", 2,
        { pla_args_type_16, pla_args_type_16 },
        { "slot", "line_nr" } },

    // 0x002b
    { "opcode_002b", 1,
        { pla_args_type_16 },
        { } },

    // 0x002c
    { "render_text_with_box", 4,
        { pla_args_type_16, pla_args_type_16, pla_args_type_16, pla_args_type_16 },
        { "slot", "wdw_idx", "x", "y" } },

    // 0x002d
    { "blit_and_dealloc", 1,
        { pla_args_type_16 },
        { "text_file" } },

    // 0x002e
    { "set_text_bounds", 3,
        { pla_args_type_16, pla_args_type_16, pla_args_type_16 },
        { "slot", "width", "height" } },

    // 0x002f
    { "opcode_002f", 2,
        { pla_args_type_16, pla_args_type_16 },
        { } },

    // 0x0030
    { },

    // 0x0031
    { "set_text_color", 4,
        { pla_args_type_16, pla_args_type_16, pla_args_type_16, pla_args_type_16 },
        { "idx", "r", "g", "b" } },

    // 0x0032
    { "change_text_color", 1,
        { pla_args_type_16 },
        { "idx" } },

    // 0x0033
    { "if_swivar", 4,
        { pla_args_type_16, pla_args_type_16, pla_args_type_32, pla_args_type_32 },
        { "swivar_idx", "kind", "value", "pla_offset" } },

    // 0x0034
    { },

    // 0x0035
    { "maybe_goto", 2,
        { pla_args_type_32, pla_args_type_16 },
        { "idx", "pla_offset" } },

    // 0x0036
    { "maybe_return", 0,
        {  },
        { } },

    // 0x0037
    { },

    // 0x0038
    { },

    // 0x0039
    { },

    // 0x003a
    { "opcode_003a", 7,
        { pla_args_type_16, pla_args_type_16, pla_args_type_16, pla_args_type_16, pla_args_type_16, pla_args_type_16, pla_args_type_16 },
        { } },

    // 0x003b
    { },

    // 0x003c
    { },

    // 0x003d
    { },

    // 0x003e
    { },

    // 0x003f
    { },

    // 0x0040
    { },

    // 0x0041
    { },

    // 0x0042
    { "boh", 12,
        { pla_args_type_16, pla_args_type_16, pla_args_type_32, pla_args_type_16, pla_args_type_16, pla_args_type_16, pla_args_type_32, pla_args_type_16, pla_args_type_16, pla_args_type_16, pla_args_type_32, pla_args_type_32 },
        { "swivar1", "boh2", "boh3", "boh4", "swivar2", "boh6", "boh7", "boh8", "swivar3", "boh10", "boh11", "pla_offset" } },


};

static const int pla_token_infos_count = sizeof(pla_token_infos) / sizeof(pla_token_info);
