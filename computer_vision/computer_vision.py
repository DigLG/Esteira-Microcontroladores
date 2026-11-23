__DEBUG = False

from object import Object
from time import sleep
from serial import Serial
from cv2 import VideoCapture, QRCodeDetector, COLOR_BGR2HSV, RETR_TREE, CHAIN_APPROX_SIMPLE, cvtColor, inRange, findContours, minAreaRect, boxPoints
if __DEBUG:
    from cv2 import imshow, drawContours, waitKey, destroyAllWindows
    from numpy import int32

picture_delay_seconds: float = 0.05

object_color_range_hsv = (
    (35, 70, 70),
    (85, 255, 255),
)

object_min_area = 50000

# __serial = Serial(port = 'COM3', baudrate = 9600, timeout = 0.01)
__video_capture = VideoCapture(0)
__qr_code_detector = QRCodeDetector()

on_object_identified: list[callable] = [
    # lambda object: __serial.write(f"{object}\n".encode('UTF-8')),
]
'''
Adicione funções nessa lista para ativarem quando um objeto for identificado (qr code e rotação).
As funções tem um parâmetro com o objeto identificado.
'''

__should_look_for_object: bool = False

__identified_objects: list[Object] = []

__last_identified_qr_code: str = None
__last_identified_rotation: list[list[int]] = None

def get_last_identified_object() -> Object | None:
    if len(__identified_objects) == 0: return None
    return __identified_objects.pop()


def get_last_identified_rotation() -> list[list[int]] | None:
    return __last_identified_rotation


def on_movement_detected():
    global __should_look_for_object
    __should_look_for_object = True


def on_object_left_detection_area():
    global __should_look_for_object
    __should_look_for_object = False


def __look_for_qr_code(image) -> str | None:
    data, _, _ = __qr_code_detector.detectAndDecode(image)
    return data if data != '' else None


def __look_for_object(image) -> list[list[int]] | None:
    hsv = cvtColor(image, COLOR_BGR2HSV)
    filtered_image = inRange(hsv, object_color_range_hsv[0], object_color_range_hsv[1])
    contours, _ = findContours(filtered_image, RETR_TREE, CHAIN_APPROX_SIMPLE)

    for contour in contours:
        rect = minAreaRect(contour)
        _, (width, height), _ = rect

        if width * height > object_min_area:
            box = boxPoints(rect)
            if __DEBUG:
                print(f'Area: {width * height}')
                imshow('Imagem filtrada', filtered_image)
                drawContours(image, int32([box]), 0, color = (0, 0, 255), thickness = 2)
                imshow('Objeto identificado', image)

            return [[int(x), int(y)] for x, y in box]

    return None


while True:
    if __DEBUG or __should_look_for_object:
        _, image = __video_capture.read()

        # if __DEBUG:
        #     print(f"{__last_identified_qr_code} - {__last_identified_rotation}")

        if __last_identified_qr_code is None:
            __last_identified_qr_code = __look_for_qr_code(image)
            if __DEBUG and __last_identified_qr_code is not None: print(f'QR Code: {__last_identified_qr_code}')

        if __last_identified_rotation is None:
            __last_identified_rotation = __look_for_object(image)
            if __DEBUG and __last_identified_rotation is not None: print(f'Pontos: {__last_identified_rotation}')

        if __DEBUG:
            imshow('Camera', image)
            if waitKey(1) & 0xFF == ord('q'):
                __video_capture.release()
                destroyAllWindows()
                break

        if __last_identified_qr_code is not None and __last_identified_rotation is not None:
            identified_object = Object(__last_identified_qr_code, __last_identified_rotation, image)

            for function in on_object_identified: function(identified_object)
            __identified_objects.append(identified_object)

            __should_look_for_object = False
            __last_identified_qr_code = None
            __last_identified_rotation = None

    sleep(picture_delay_seconds)
