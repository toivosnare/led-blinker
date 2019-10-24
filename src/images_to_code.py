from PIL import Image

IMAGE_NAMES = ["0001.png", "0002.png", "0003.png", "0004.png", "0005.png", "0006.png", "0007.png",
                "0008.png", "0009.png", "0010.png", "0011.png", "0012.png", "0013.png", "0014.png",
                "0015.png", "0016.png", "0017.png", "0018.png", "0019.png"]

LED_SIDE_LEN = 16
SET_LED_THRESHOLD = 50

# Returns hex number without the prefix "0x".
def num_to_hex(number):
  return hex(number)[2:]


# Takes integer which is divided by 15 for accuracy of 15 ms.
def wait_command(wait_time):
  if wait_time < 65536:
    hex_time = num_to_hex(int(wait_time / 15))
    return "w0000"[:5-len(hex_time)] + hex_time
  return "wffff"


def set_led_effect_command(x, y):
  return "s" + num_to_hex(y) + num_to_hex(x)


def main():
  str_imgs = []
  images = []
  for name in IMAGE_NAMES:
    try:
      images.append(Image.open(name))
    except IOError:
      print("Couldn't open", name)
  for img in images:
    str_img = ""
    for y in range(LED_SIDE_LEN):
      for x in range(LED_SIDE_LEN):
        if img.getpixel((x, y))[0] > SET_LED_THRESHOLD:
          str_img += set_led_effect_command(x, y)
    str_imgs.append(str_img)
  
  i = 0
  for img in str_imgs:
    i += 1
    print("// {}:\n\"{}\"".format(i, img))


main()
