#pragma once

#include<hal/peripherals/keyboard/layouts/common.h>
#include<hal/peripherals/keyboard/layouts/en_US.h>

typedef enum {
    KB_LAYOUT_en_US=0
} keyboard_layouts;

static keyboard_layout_map* keyboard_layout_lookup[]={
    &en_US
};