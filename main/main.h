#pragma once

#include <iostream>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_mac.h"

class Main final {
    public:
        void run(void);
        void setup(void);
}; 