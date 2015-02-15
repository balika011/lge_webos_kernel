MT53xx Nucleus driver demo program UI converion tools
[1] UI storage representation:
    The raw bytes of UI background and icons are stored in an UINT16 array,
which sequentially holds the raw bytes (represented in C format) of the following bitmaps.
    menu_bg, a 16-bpp bitmap with resolution 808 * 412
    icon_video, a 16-bpp bitmap with resolution 192 * 48
    icon_video_off
    icon_color
    icon_color_off
    icno_sound
    icon_sound_off
    icon_setup
    icon_setup_off
    icon_multimedia
    icon_multimedia_off
    info_osd, a 16-bpp bitmap with resolution 808 * 98
    So the size of the array is 808 * 412 + 192 * 48 * 10 + 808 * 98 = 504240 elements = 1008480 bytes 

[2] How to converted from bitmap files to demo program's UI storage?    
    1. The bitmap files of UI background and icons are converted into raw bytes.    2. The raw bytes of UI background and icons (including titles) are concated together, in the following order. 
       menu_bg
       menutitle_video
       menutitle_video_off
       menutitle_color
       menutitle_color_off
       menutitle_sound
       menutitle_sound_off
       menutitle_setup
       menutitle_setup_off
       menutitle_multimedia
       menutitle_multimedia_off
       info_osd 
    3. The concated raw bytes are converted to an UINT16 array and the contain of the array should be put in demo/nav_ui.i.

[2] Tools
    nav_ui_raw2header: Convert the raw bytes to an UINT16 array.
    nav_ui_header2raw: Convert the UINT16 array to raw bytes.
    nav_ui_replace_raw: Substitute the raw bytes in the range with the raw bytes of the input file.

