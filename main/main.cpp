#include "main.h"

Main App;

// PWM definitions
#define PWM_CHANNEL LEDC_CHANNEL_0
#define LED_PWM_CHANNEL LEDC_CHANNEL_1
#define PWM_TIMER LEDC_TIMER_0
#define PWM_FREQ 5000       // Frequency in Hz
#define PWM_RES LEDC_TIMER_8_BIT  // Resolution (8-bit: 0-255)

// pin definitions
#define TRIGGER_PIN GPIO_NUM_27
#define MOTOR_PIN GPIO_NUM_5
#define LED_PIN GPIO_NUM_21

void Main::setup(void) {   
    // trigger pin configuration
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;  
    io_conf.pin_bit_mask = (1ULL << TRIGGER_PIN); 
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE; 
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;      
    gpio_config(&io_conf);                     

    // motor PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .duty_resolution  = PWM_RES,
        .timer_num        = PWM_TIMER,
        .freq_hz          = PWM_FREQ,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // motor PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .gpio_num       = MOTOR_PIN,
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = PWM_CHANNEL,
        .timer_sel      = PWM_TIMER,
        .duty           = 0,         // Start with 0% duty cycle
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    // LED PWM channel configuration
    ledc_channel_config_t ledc_led_channel = {
        .gpio_num       = LED_PIN,
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = LED_PWM_CHANNEL,
        .timer_sel      = PWM_TIMER,
        .duty           = 0,         // Start with 0% duty cycle
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_led_channel));    

}

void Main::run(void) {
    // trigger
    int currentButtonState = gpio_get_level(TRIGGER_PIN);

    while (currentButtonState == 0) {

        // motor
        for (int duty = 0; duty <= 255; duty++) {
            // motor speed
            ledc_set_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL, duty); // using ledc_* for PWM control
            ledc_update_duty(LEDC_LOW_SPEED_MODE, PWM_CHANNEL);

            // led brightness
            ledc_set_duty(LEDC_LOW_SPEED_MODE, LED_PWM_CHANNEL, duty);
            ledc_update_duty(LEDC_LOW_SPEED_MODE, LED_PWM_CHANNEL);
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        // rereading trigger pin state (can break loop)
        currentButtonState = gpio_get_level(TRIGGER_PIN);
    }
}

extern "C" void app_main(void) {
    App.setup();

    while (true){
        App.run();
    }
}