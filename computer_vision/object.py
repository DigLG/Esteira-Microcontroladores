from cv2.typing import MatLike

class Object:
    qr_code: str
    'Conteudo do qr code do objeto.'

    rotation_points: list[list[int]]
    'Cada vetor (x, y) de um ponto do objeto visto de cima.'

    image: MatLike

    def __init__(self, qr_code: str, rotation_points: list[list[int]], image: MatLike):
        self.qr_code = qr_code
        self.rotation_points = rotation_points
        self.image = image


    def __str__(self) -> str:
        return f'qr_code={self.qr_code}. rotation_points={self.rotation_points}.'
