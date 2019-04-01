from PIL import Image
import pytesseract
y=pytesseract.image_to_string(Image.open('A.jpg'), config='--psm 8 --oem 3 -c tessedit_char_whitelist=ABCDEF')
print (y)