// barcodedecoder.cpp
#include "barcodedecoder.h"
#include <zbar.h>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include "product.h"
#include <QDebug>

QList<DecodedBarcode> decodeBarcodes(const QImage &image)
{
    QList<DecodedBarcode> results;

    QImage gray = image.convertToFormat(QImage::Format_Grayscale8);

    // ZBar needs tightly packed rows; QImage can pad each row, so repack manually
    QByteArray packed;
    packed.reserve(gray.width() * gray.height());
    for (int y = 0; y < gray.height(); ++y) {
        packed.append(reinterpret_cast<const char *>(gray.constScanLine(y)), gray.width());
    }

    zbar::Image zImage(gray.width(), gray.height(), "Y800",
                       packed.constData(), packed.size());

    zbar::ImageScanner scanner;
    scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);
    scanner.scan(zImage);

    for (auto it = zImage.symbol_begin(); it != zImage.symbol_end(); ++it) {
        results.append({ QString::fromStdString(it->get_type_name()),
                        QString::fromStdString(it->get_data()) });
    }

    return results;
}

Product *searchProductInDataBase(QList<DecodedBarcode> code)
{
    QString productCode = code.first().data;
    QFile databaseFile(":/database.csv");

    QTextStream content(&databaseFile);

    if (!databaseFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open file for reading:" << databaseFile.errorString();
        return NULL;
    }

    while (!content.atEnd()) {
        QString product = content.readLine();
        QStringList field = product.split(',');

        if (field[1] == productCode) {
            qDebug() << "Product Found!!";
            Product *newProduct{new Product(field[1],
                                            field[2],
                                            field[3],
                                            field[4],
                                            field[5].toFloat(),
                                            field[6],
                                            field[8],
                                            field[9],
                                            field[10],
                                            field[11])};
            return newProduct;
        }
    }

    databaseFile.close();

    return NULL;
}