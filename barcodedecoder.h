// barcodedecoder.h
#ifndef BARCODEDECODER_H
#define BARCODEDECODER_H

#include <QImage>
#include <QString>
#include <QList>
#include "product.h"

struct DecodedBarcode {
    QString type;
    QString data;
};

QList<DecodedBarcode> decodeBarcodes(const QImage &image);
Product *searchProductInDataBase(QList<DecodedBarcode> code);

#endif