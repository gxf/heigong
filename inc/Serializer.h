#ifndef SERIALIZER_H
#define SERIALIZER_H

class Serializer{
    public:
        virtual void Serialize() = 0;
        virtual void Deserialize() = 0;
};

#endif
