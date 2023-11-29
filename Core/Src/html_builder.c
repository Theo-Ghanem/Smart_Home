/*
 * html_builder.c
 *
 *  Created on: Nov 15, 2023
 *      Author: Philippe
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


const char html[] = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\"><title>Document</title><meta http-equiv=\"refresh\" content=\"10\"></head><body><h1>Smart Home System - Online</h1><div class=\"card\"><div class=\"title\"><h2>Alarm</h2><label class=\"switch\"><input type=\"checkbox\" id=\"alarm-checkbox\"                 > <span class=\"slider round\"></span></label></div><div id=\"status-nothing\" class=\"alarm                  \">Nothing to report</div><div id=\"status-intruder\" class=\"alarm intruder              \">INTRUDER DETECTED!</div></div><div class=\"card\"><div class=\"title\"><h2>Sensors</h2></div><div class=\"sensors\"><div class=\"sensor\"><div class=\"thermometer\"><div class=\"stem\"><div class=\"stem-perct\"></div></div><div class=\"bulb\"></div></div><div class=\"sens-value\">                   </div></div><div class=\"sensor\"><div class=\"humsensor\"><div class=\"droplet\"></div></div><div class=\"sens-value\">                      </div></div><div class=\"sensor barsensor\"><div class=\"barometer\"></div><div class=\"sens-value\">                   </div></div></div></div></body></html><style>*,::after,::before{box-sizing:border-box}html{--background-color:#303030;--color-accent-extra-light:#437e56;--color-accent-light:#2a4e36;background-color:var(--background-color);color:#fff;font-family:sans-serif}h2{margin:0}.card{max-width:30rem;padding:1.5rem;background-color:var(--color-accent-extra-light);border-radius:1rem;margin-bottom:2rem}.title{display:flex;align-items:center;justify-content:space-between;margin-bottom:1rem}.switch{position:relative;display:inline-block;--switch-height:1.5rem;--switch-width:3rem;width:var(--switch-width);height:var(--switch-height)}.switch input{opacity:0;width:0;height:0}.slider{position:absolute;cursor:pointer;top:0;left:0;right:0;bottom:0;background-color:#ccc;-webkit-transition:.4s;transition:.4s}.slider:before{position:absolute;content:\"\";height:calc(var(--switch-height) - .2rem);width:calc(var(--switch-height) - .2rem);left:.09rem;bottom:.11rem;background-color:#fff;-webkit-transition:.4s;transition:.4s}.alarm{display:none}.alarm.active{display:block}.alarm.intruder{font-weight:700;color:#8b0000}input:checked+.slider{background-color:#2196f3}input:focus+.slider{box-shadow:0 0 1px #2196f3}input:checked+.slider:before{-webkit-transform:translateX(calc(var(--switch-width) - var(--switch-height)));-ms-transform:translateX(calc(var(--switch-width) - var(--switch-height)));transform:translateX(calc(var(--switch-width) - var(--switch-height)))}.slider.round{border-radius:calc(var(--switch-height)/ 2)}.slider.round:before{border-radius:50%}.thermometer{display:block;position:relative;height:100px;width:30px;flex-shrink:0}.bulb{display:inline-block;border:2px solid #222;width:25px;height:25px;border-radius:50%;position:absolute;left:0;bottom:0;box-shadow:0 0 0 2px #fff inset;background:red}.stem{display:inline-block;border:2px solid #222;width:14px;height:80px;border-radius:0 0 20px 20px;border-top:2px solid transparent;position:absolute;bottom:20.5px;left:5px;box-shadow:0 0 0 2px #fff inset;transform:rotate(180deg);z-index:10;padding-bottom:3px}.stem-perct{height:calc(30% + 6px);width:6px;background:red;display:block;margin:0 auto;border-radius:10px;transform:translateY(-5px)}.humsensor{display:flex}.droplet{position:relative;width:3rem;height:3rem;background-color:transparent;overflow:hidden;border:solid #fff 3px;border-radius:0 50% 50% 50%;transform:rotateZ(45deg);display:flex;align-items:center;justify-content:center}.droplet::after{content:\"\";position:absolute;bottom:-.5rem;left:-3rem;right:-3rem;height:50%;background-color:#2196f3;transform:rotateZ(-45deg) translateX(10px)}.barsensor{width:100%!important}.barometer{position:relative;width:3rem;height:3rem;border:solid #222 3px;border-radius:100%}.barometer::after{content:\"\";position:absolute;left:50%;right:50%;bottom:50%;top:.3rem;border:solid #222;border-width:0 3px 0 0}.sensors{display:flex;flex-wrap:wrap;gap:1rem}.sensor{background-color:rgba(0,0,0,.1);border-radius:1rem;width:calc(50% - .5rem);padding:1rem;justify-content:center;display:flex;align-items:center}.sens-value{font-size:2rem;margin-left:1rem}</style><script>document.getElementById(\"alarm-checkbox\").addEventListener('change', (event) => { if (event.currentTarget.checked) { document.getElementById(\"status-nothing\").classList.add(\"active\"); fetch(\"/?radio=1\", { method: \"POST\" }); } else { document.getElementById(\"status-nothing\").classList.remove(\"active\"); document.getElementById(\"status-intruder\").classList.remove(\"active\"); fetch('/?radio=0', { method: \"POST\" }); } });</script>";

char* inject(int alarmEnabled, int intruderDetected, int temp, int pres, int humd){
    char* injected = malloc(sizeof(char) * strlen(html));
    strcpy(injected, html);

    strncpy(&injected[353], alarmEnabled ? "checked" : "       ", 7);

    if(alarmEnabled){
    	if(intruderDetected){
    		strncpy(&injected[547], "active", 6);
    	} else {
    		strncpy(&injected[457], "active", 6);
    	}
    }

    char* tempStr = calloc(1, sizeof(char) * 8);
    sprintf(tempStr, "%d&deg;C", temp);
    strncpy(&injected[825], tempStr, strlen(tempStr));

    char* humdStr = calloc(1, sizeof(char) * 8);
    sprintf(humdStr, "%d%%", humd);
    strncpy(&injected[956], humdStr, strlen(humdStr));

    char* presStr = calloc(1, sizeof(char) * 16);
    sprintf(presStr, "%d hPa", pres);
    strncpy(&injected[1073], presStr, strlen(presStr));

    return injected;
}
