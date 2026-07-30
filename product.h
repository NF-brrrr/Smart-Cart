#ifndef PRODUCT_H
#define PRODUCT_H
#include <QString>
#include <QList>

class Product
{
public:
    Product(QString code,
            QString name,
            QString category,
            QString brand,
            float price,
            QString quantity,
            QString manufacturer,
            QString supplier,
            QString expiry_date,
            QString manufacture_date);
    QString code;
    QString name;
    QString category;
    QString brand;
    float price;
    QString quantity;
    QString manufacturer;
    QString supplier;
    QString expiry_date;
    QString manufacture_date;
    int inCart;
};

#endif // PRODUCT_H
