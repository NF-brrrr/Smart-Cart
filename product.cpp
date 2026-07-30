#include "product.h"

Product::Product(QString code,
                 QString name,
                 QString category,
                 QString brand,
                 float price,
                 QString quantity,
                 QString manufacturer,
                 QString supplier,
                 QString expiry_date,
                 QString manufacture_date)
{
    this->code = code;
    this->name = name;
    this->category = category;
    this->brand = brand;
    this->price = price;
    this->quantity = quantity;
    this->manufacturer = manufacturer;
    this->supplier = supplier;
    this->expiry_date = expiry_date;
    this->manufacture_date = manufacture_date;
}