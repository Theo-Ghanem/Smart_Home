/*
 * html_builder.c
 *
 *  Created on: Nov 15, 2023
 *      Author: Philippe
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


const char html[] = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\"><title>Document</title></head><body><h1>Smart Home System - Online</h1><div class=\"card\"><div class=\"title\"><h2>Alarm</h2><label class=\"switch\"><input type=\"checkbox\" id=\"alarm-checkbox\"              > <span class=\"slider round\"></span></label></div><div>Status: Nothing to report</div></div><div class=\"card\"><div class=\"title\"><h2>Temperature</h2></div><div class=\"temp\"><div class=\"thermometer\"><div class=\"stem\"><div class=\"stem-perct\"></div></div><div class=\"bulb\"></div></div><div class=\"temp-value\">                 </div></div></div></body></html><style>html{--background-color:#303030;--color-accent-extra-light:#437e56;--color-accent-light:#2a4e36;background-color:var(--background-color);color:#fff;font-family:sans-serif}h2{margin:0}.card{max-width:30rem;padding:1.5rem;background-color:var(--color-accent-extra-light);border-radius:1rem;margin-bottom:2rem}.title{display:flex;align-items:center;justify-content:space-between;margin-bottom:1rem}.switch{position:relative;display:inline-block;--switch-height:1.5rem;--switch-width:3rem;width:var(--switch-width);height:var(--switch-height)}.switch input{opacity:0;width:0;height:0}.slider{position:absolute;cursor:pointer;top:0;left:0;right:0;bottom:0;background-color:#ccc;-webkit-transition:.4s;transition:.4s}.slider:before{position:absolute;content:\"\";height:calc(var(--switch-height) - .2rem);width:calc(var(--switch-height) - .2rem);left:.09rem;bottom:.11rem;background-color:#fff;-webkit-transition:.4s;transition:.4s}input:checked+.slider{background-color:#2196f3}input:focus+.slider{box-shadow:0 0 1px #2196f3}input:checked+.slider:before{-webkit-transform:translateX(calc(var(--switch-width) - var(--switch-height)));-ms-transform:translateX(calc(var(--switch-width) - var(--switch-height)));transform:translateX(calc(var(--switch-width) - var(--switch-height)))}.slider.round{border-radius:calc(var(--switch-height)/ 2)}.slider.round:before{border-radius:50%}.thermometer{display:block;position:relative;height:100px}.bulb{display:inline-block;border:2px solid #222;width:25px;height:25px;border-radius:50%;position:absolute;left:0;bottom:0;box-shadow:0 0 0 2px #fff inset;background:red}.stem{display:inline-block;border:2px solid #222;width:14px;height:80px;border-radius:0 0 20px 20px;border-top:2px solid transparent;position:absolute;bottom:24px;left:5px;box-shadow:0 0 0 2px #fff inset;transform:rotate(180deg);z-index:10;padding-bottom:3px}.stem-perct{height:calc(             + 6px);width:10px;background:red;display:block;margin:0 auto;border-radius:10px;transform:translateY(-5px)}.temp{width:100%;padding:3rem 0;justify-content:center;display:flex;align-items:center}.temp-value{font-size:2rem;margin-left:3rem}</style><script>document.getElementById(\"alarm-checkbox\").addEventListener('change', (event) => {\n    if (event.currentTarget.checked) {\n      fetch(\"/?radio=1\", { method: \"POST\" });\n    } else {\n      fetch('/?radio=0', { method: \"POST\" });\n    }\n  });</script>";

char* inject(int alarmEnabled, int temp, int thermPercent){
    char* injected = malloc(sizeof(char) * strlen(html));
    strcpy(injected, html);

    strncpy(&injected[313], alarmEnabled ? "checked" : "       ", 7);

    char* tempStr = malloc(sizeof(char) * 20);
    sprintf(tempStr, "%d&deg;C", temp);
    strncpy(&injected[632], tempStr, strlen(tempStr) - 1);

    char* thermPercentStr = malloc(sizeof(char) * 20);
    sprintf(thermPercentStr, "%d%s", thermPercent, "%%");
    strncpy(&injected[2569], thermPercentStr, strlen(thermPercentStr) - 1);

    return injected;
}
