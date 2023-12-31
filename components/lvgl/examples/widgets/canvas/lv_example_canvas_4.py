import fs_driver

CANVAS_WIDTH  = 50
CANVAS_HEIGHT = 50

LV_COLOR_SIZE = 32

#
# Draw a text to the canvas
#

# Create a buffer for the canvas
cbuf = bytearray((LV_COLOR_SIZE // 8) * CANVAS_WIDTH * CANVAS_HEIGHT)

# Create a canvas and initialize its palette
canvas = lv.canvas(lv.screen_active())
canvas.set_buffer(cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, lv.COLOR_FORMAT.NATIVE)
canvas.fill_bg(lv.color_hex3(0xccc), lv.OPA.COVER)
canvas.center()

dsc = lv.draw_label_dsc_t()
dsc.init()

dsc.color = lv.palette_main(lv.PALETTE.RED)

# get the directory in which the script is running
try:
    script_path = __file__[:__file__.rfind('/')] if __file__.find('/') >= 0 else '.'
except NameError:
    print("Could not find script path")
    script_path = ''

if script_path != '':
    try:
        dsc.font = lv.font_montserrat_18
    except:
        # needed for dynamic font loading
        fs_drv = lv.fs_drv_t()
        fs_driver.fs_register(fs_drv, 'S')

        print("Loading font montserrat_18")
        font_montserrat_18 = lv.font_t()
        lv.binfont_load(font_montserrat_18, "S:" + script_path + "/../../assets/font/montserrat-18.fnt")
        if not font_montserrat_18:
            print("Font loading failed")
        else:
            dsc.font = font_montserrat_18

dsc.decor = lv.TEXT_DECOR.UNDERLINE
dsc.text = "Hello"


layer = lv.layer_t()
canvas.init_layer(layer);


coords = lv.area_t()
coords.x1 = 10
coords.y1 = 10
coords.x2 = 40
coords.y2 = 30


lv.draw_label(layer, dsc, coords)

canvas.finish_layer(layer)


