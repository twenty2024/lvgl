#!/usr/bin/python3

import os, sys


hebrew_syms = "קורא נלהב שצובר אוסף עצום של ספרים יקרים"
devanagari_syms = "हरित कार्यकर्ता, एक स्थायी कल के लिए प्रयासरत।"
#arabic_syms = "عاشق تاریخ و عاشق همه چیز عتیقه فيلم برتقالي وناقد سينمائي عرضي"
arabic_range = "0xfe70-0xfeff,0xfb50-0xfdcf"
russian_syms = "Любитель приключений на свежем воздухе, всегда ищущий новых острых ощущений"
chinese_syms = "对编程和技术充满热情。 开源倡导者。"
latin_range = "0x20-0x24f"

cmd = f"lv_font_conv --no-compress --no-prefilter --bpp 4 --size 15 --format lvgl --output font_multilang_small.c"
cmd = cmd +  f" --font NotoSerifHebrew-SemiBold.ttf --symbols \"{hebrew_syms}\""
cmd = cmd +  f" --font NotoSerifDevanagari-SemiBold.ttf --symbols \"{devanagari_syms}\""
cmd = cmd +  f" --font NotoNaskhArabic-SemiBold.ttf   -r \"{arabic_range}\""
cmd = cmd +  f" --font NotoSansSC-Medium.otf --symbols \"{chinese_syms}\""
cmd = cmd +  f" --font Montserrat-SemiBold.ttf -r {latin_range} --symbols \"{russian_syms}\""


print(cmd)
os.system(cmd)

cmd = f"lv_font_conv --no-compress --no-prefilter --bpp 4 --size 22 --format lvgl --output font_multilang_large.c --font Montserrat-Bold.ttf -r 0x20-0x7F"

print(cmd)
os.system(cmd)






