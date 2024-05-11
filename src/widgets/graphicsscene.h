#pragma once

#include <qgraphicsscene.h>
#include <qgraphicssceneevent.h>
#include <qgraphicsview.h>
#include <qlogging.h>
#include <qpoint.h>

inline constexpr int kCellSize{50};
inline constexpr int kDefaultContentViewSize{3};
inline constexpr int kGridWidth{10};
inline constexpr QColor kGridColor{QColor{0, 0, 0}};

class GraphicsScene : public QGraphicsScene {
    Q_OBJECT

   public:
    explicit GraphicsScene(QWidget* parent, int size, int cell_size)
        : QGraphicsScene(parent),
          point_{0, 0},
          height_{size},
          width_{size},
          cell_size_{cell_size} {
        // handle wrong size values
        if (size < 1) {
            height_ = 1;
            width_ = 1;
        }

        grid_pen_ = new QPen{kGridColor, kGridWidth};

        drawGrid();

        QObject::connect(this, &GraphicsScene::mousePressed, this,
                         &GraphicsScene::onMousePress);
    };

    // Set new size and redraw
    void resize(int height, int width) {
        if (height < 1) height = 1;
        if (width < 1) width = 1;

        height_ = height;
        width_ = width;

        clear();
        drawGrid();
    }
    // Set new size and redraw
    void resize(int size) {
        if (size < 1) size = 1;

        height_ = size;
        width_ = size;

        clear();
        drawGrid();
    }

    QPointF lastMousePressPos() { return point_; }

   signals:
    void mousePressed();

   protected slots:
    void onMousePress() {
        qDebug() << point_;
        get_cell_rect(point_.x(), point_.y());
    }

   protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* e) override {
        QPointF point{e->scenePos()};
        // // Out of bounds check
        // if (point.x() >= 0 && point.y() >= 0 &&
        //     point.x() <= (width_ * (cell_size_ + grid_pen_->width())) &&
        //     point.y() <= (height_ * (cell_size_ + grid_pen_->width()))) {
        point_ = point;
        emit mousePressed();
        // }
    }

    void drawGrid() {
        int grid_width{grid_pen_->width()};
        // Draw vercital lines
        for (int i{}; i <= width_; ++i) {
            addLine(cell_size_ * i + grid_width * i, 0,
                    cell_size_ * i + grid_width * i,
                    cell_size_ * height_ + grid_width * height_, *grid_pen_);
        }
        // Draw horisontal lines
        for (int i{}; i <= height_; ++i) {
            addLine(0, cell_size_ * i + grid_width * i,
                    cell_size_ * width_ + grid_width * width_,
                    cell_size_ * i + grid_width * i, *grid_pen_);
        }

        // addRect(0, 0, kCellSize + kGridWidth, kCellSize + kGridWidth,
        //         QPen(QColor(255, 255, 255), kGridWidth));
    }

    QRectF get_cell_rect(int x, int y) {
        int grid_width{grid_pen_->width()};

        addRect(x, y, kCellSize + kGridWidth, kCellSize + kGridWidth,
                QPen(QColor(255, 255, 255), kGridWidth));

        return QRectF{0, 0, 0, 0};
    }

    QPointF point_;
    int height_{};
    int width_{};
    int cell_size_{};
    QPen* grid_pen_{};
};