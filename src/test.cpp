#include <iostream>
#include <vector>

// Определяем структуру для трехмерной точки
struct Point3D {
    double x, y, z;
};

// Функция для нахождения точки пересечения двух отрезков
Point3D findIntersection(const std::vector<Point3D>& segment1, const std::vector<Point3D>& segment2) {
    // Получаем точки первого отрезка
    const Point3D& point1 = segment1[0];
    const Point3D& point2 = segment1[1];

    // Получаем точки второго отрезка
    const Point3D& point3 = segment2[0];
    const Point3D& point4 = segment2[1];

    // Вычисляем вектор направления от первой точки ко второй точке первого отрезка
    Point3D direction1 = {point2.x - point1.x, point2.y - point1.y, point2.z - point1.z};

    // Вычисляем вектор направления от третьей точки к четвертой точке второго отрезка
    Point3D direction2 = {point4.x - point3.x, point4.y - point3.y, point4.z - point3.z};

    // Вычисляем вектор перпендикулярный направлению первого отрезка
    Point3D perpendicular1 = {-direction1.y, direction1.x, 0};

    // Вычисляем вектор перпендикулярный направлению второго отрезка
    Point3D perpendicular2 = {-direction2.y, direction2.x, 0};

    // Вычисляем параметры t и u для нахождения точки пересечения
    double t = ((point3.x - point1.x) * perpendicular2.x + (point3.y - point1.y) * perpendicular2.y + (point3.z - point1.z) * perpendicular2.z) / (direction1.x * perpendicular2.x + direction1.y * perpendicular2.y + direction1.z * perpendicular2.z);
    double u = ((point1.x - point3.x) * perpendicular1.x + (point1.y - point3.y) * perpendicular1.y + (point1.z - point3.z) * perpendicular1.z) / (direction2.x * perpendicular1.x + direction2.y * perpendicular1.y + direction2.z * perpendicular1.z);

    // Вычисляем точку пересечения
    Point3D intersection = {point1.x + t * direction1.x, point1.y + t * direction1.y, point1.z + t * direction1.z};

    return intersection;
}

int main() {
    // Создаем два вектора из двух точек, образующих первый отрезок
    std::vector<Point3D> segment1 = {{0, 1, 0}, {1, 1, 0}};

    // Создаем два вектора из двух точек, образующих второй отрезок
    std::vector<Point3D> segment2 = {{1, 1, 0}, {2, 1, 0}};

    // Находим точку пересечения двух отрезков
    Point3D intersection = findIntersection(segment1, segment2);

    // Выводим точку пересечения
    std::cout << "(" << intersection.x << ", " << intersection.y << ", " << intersection.z << ")" << std::endl;

    return 0;
}
