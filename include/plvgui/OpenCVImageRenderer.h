#ifndef OpenCVImageRenderer_H
#define OpenCVImageRenderer_H

#include <QMutex>
#include <QLabel>

#include "OpenCVImage.h"
#include "QtImage.h"
#include "RefPtr.h"
#include "ImageConverter.h"
#include "DataRenderer.h"


class QPixmap;
class QLabel;
class QVBoxLayout;
class QImage;

namespace plv {
    class Data;
}

using namespace plv;

namespace plvgui {

    class OpenCVImageRenderer : public DataRenderer
    {
        Q_OBJECT

    public:
        OpenCVImageRenderer(QWidget* parent);
        virtual ~OpenCVImageRenderer() {}
        QSize sizeHint() const;
        int heightForWidth(int w) const;

    private:
        void putImage();
        QLabel*         m_imagelabel;
        QVBoxLayout*    m_layout;
        bool            m_busy;
        QMutex          m_busy_mutex;
        RefPtr<ImageConverter> m_converter;

    public slots:
        virtual void newData( RefPtr<Data> data );
        void updateImage( RefPtr<QtImage> img );

    };

    class ImageLabel : public QLabel
    {
    public:
        ImageLabel(QWidget* parent);
        QSize sizeHint() const;
        QSize minimumSizeHint() const;
        int heightForWidth(int w) const;
    };
}
#endif // OpenCVImageRenderer_H
