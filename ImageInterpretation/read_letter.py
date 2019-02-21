from PIL import Image
import pytesseract
y = pytesseract.image_to_string(Image.open('TEST.jpg'), config='--psm 10 --oem 3 -c tessedit_char_whitelist=ABCDEF')

print (y)