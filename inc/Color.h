#ifndef COLOR_H
#define COLOR_H

class Color{
    public:
        unsigned char R;
        unsigned char G;
        unsigned char B;
    public:
        Color(int r, int g, int b)
        {
            Clamp(r, g, b);
        }
    public:
        inline unsigned char Get8BitGray(){
            return (unsigned char)(
                    ( (double)R/(255 * 3) +
                      (double)G/(255 * 3) +
                      (double)B/(255 * 3)
                    ) * 0xff);
        }
        inline void Clamp(int r, int g, int b){
            if ( r > 255) R = 255;
            else if ( r < 0) R = 0;
            else R = r;

            if ( g > 255) G = 255;
            else if ( g < 0) G = 0;
            else G = g;

            if ( b > 255) B = 255;
            else if ( b < 0) B = 0;
            else B = b;
        }
};

class Color_A : public Color{
    public:
        unsigned char A;
};
#endif
