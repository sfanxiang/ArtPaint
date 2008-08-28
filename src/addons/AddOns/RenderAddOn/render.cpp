#include "LightSource.h"
#include "Surface.h"

// Function prototypes :

// void set_pal(unsigned char *, int);
uint8 *render(float **,uint8 *, Surface *,LightSource *,int32,int32);


//int main(int argc,char *argv[]) {
//
//	struct Screen *My_screen = new Screen;
//
//	LightSource *L = new LightSource;
//    L->SetParameters( 128.0, 300.0, 128.0, 220.0, 1.0, 220.0, 128.0, 280, 128.0,
//                        1.0, 32);
//
//	float **hmap = new float*[257];
//	for(int i=0;i<257;++i) {
//		hmap[i] = new float[257];
//	}
//
//	double angle = 3.14159265 / 180.0;
//	for(int i=0;i<257;++i)
//		for(int j=0;j<257;++j) {
////              hmap[i][j] = 128 + (127.0*sin(angle*(double)j)*sin(angle*(double)i));
////            hmap[i][j] = pow(hmap[i][j],1.1);
//            hmap[i][j] = 0.0;
//        }
//                                // x    y   spec spec_n diff ambient
//    Surface *S = new Surface(hmap, 256, 256,0.5, 100, 0.9, 0.1);
//
//	uint8 *Texture = new uint8[256*256*4];
//	int tp = 0;
//	for(int i=0;i<256;++i)
//        for(int j=0;j<256;++j) {
////         Texture[tp] = (uint8)(255.0*sin(angle*(double)j)*cos(angle*(double)i));
//           Texture[tp] = 255;
//            Texture[tp+1] = 255;
//            Texture[tp+2] = (uint8)255-i;
//            tp+=4;
//		}
//    printf("Here we go...\n");
//	render(hmap,Texture,S,L,257,257);
//
//	My_screen->mode=0x115;	 /* 800 x 600 x 32 bits */
//	Open_Screen(My_screen);
//
//	unsigned int *screenPointer = (unsigned int *) My_screen->buffer;
//    uint8 *scp = (uint8 *)My_screen->buffer;
//   // screenPointer[j+(y*800)] = 0x5f;
//
//	tp = 0;
//	int i = 0;
//    unsigned int color;
//    for(int y=0;y<256;++y) {
//        for(int x=0;x<256;++x) {
//            color= 0 | ( Texture[tp+2] << 16) | (Texture[tp+1] << 8 ) | (Texture[tp] );
//            screenPointer[y*800+x] = color;
//            ++i; tp+=4;
//		}
//	}
//
//	Update_Screen(My_screen);
//
//	while(!kbhit()){ }
//	Close_Screen(My_screen);
//
//	printf("X: %d Y: %d \n",My_screen->X_Size,My_screen->Y_Size);
//
//	delete(My_screen);
//	return 0;
//}

uint8 *render(float **hm,uint8 *t,Surface *s,LightSource *l,int32 X,int32 Y) {
       	float *nx = new float[X];
        float *ny = new float[X];
        float *nz = new float[X];

        int pos = 0;
        float pixelR,pixelG,pixelB;
        float pixelIntens=0;
        float diffuse=0,specular=0;
        float v1x=0,v1y=0,v1z=1;
        float v2x=1,v2y=0,v2z=0;
        float tnx=0,tny=0,tnz=0;    // temp normal
        float hx=0,hy=0,hz=0;       // reflection vector (h:halfway)

        float lpx=0,lpy=0,lpz=0;    // light position
        float lx=0,ly=0,lz=0;       // light vector
        float ldx=0,ldy=0,ldz=0;    // light target direction vector
        float lr=0,lg=0,lb=0;       // light color
        float lightIntens=0;        // light intensity
//        float lfx,lfy,lfz;          // light focus
        float lconc=0;              // light concentration
        float eyex=0,eyey=1,eyez=0; // vector to eye constant (0,1,0)
        float length=0;

        float ka = s->GetAmbientReflection();
        float kd = s->GetDiffuse();
        float ks = s->GetSpecularity();
        float sn = s->GetSpecularityBrightness();

        for(int y=0;y<Y-1;++y) {

            for(int x=0;x<X-1;++x) {
                v1y = hm[y+1][x] - hm[y][x];
                v2y = hm[y][x+1] - hm[y][x];
                tnx = (v1y*v2z) - (v1z*v2y); // v1 cross v2
                tny = (v1z*v2x) - (v1x*v2z);
                tnz = (v1x*v2y) - (v1y*v2x);
                length=sqrt( (tnx*tnx) + (tny*tny)+ (tnz*tnz) );
                nx[x] =tnx / length; ny[x] =tny / length; nz[x] =tnz / length;
            }

            // For each light...
            for(int lightCount=0;lightCount<1;++lightCount) {
                lpx = l->x; lpy = l->y; lpz = l->z;
                lr = l->red / 255.0; lg = l->green / 255.0;
                lb = l->blue / 255.0;

                ldx = l->focusX - lpx;
                ldy = l->focusY - lpy; ldz = l->focusZ - lpz;
                length = sqrt( (ldx*ldx)+ (ldy*ldy) + (ldz*ldz) );
                ldx /=length; ldy /=length; ldz /=length;

                lconc = l->concentration;

                for(int scanx=0;scanx<X-1;++scanx) {
                    tnx = nx[scanx]; tny = ny[scanx]; tnz = nz[scanx];
                    lx = lpx-(float)scanx;
                    ly = lpy -hm[y][scanx];
                    lz = lpz -(float)y;
                    length = sqrt( (lx*lx)+ (ly*ly) + (lz*lz) );
                    lx /=length; ly /=length; lz /=length;
                    lightIntens = l->intensity *
                    pow( (-lx*ldx)+(-ly*ldy)+(-lz*ldz) ,lconc);

                    diffuse = (lx*tnx) + (ly*tny) + (lz*tnz);
                    hx = lx + eyex;
                    hy = ly + eyey;
                    hz = lz + eyez;
                    length = sqrt( (hx*hx)+ (hy*hy) + (hz*hz) );
                    hx /=length; hy /=length; hz /=length;

                    specular = lightIntens * ks * 255.0 *
                    pow( (tnx*hx)+(tny*hy)+(tnz*hz),sn);
                    diffuse  = diffuse * lightIntens * kd;

                    pixelB = t[pos];pixelG = t[pos+1];pixelR = t[pos+2];

                    pixelB = ka*pixelB + (pixelB * diffuse*lb) + specular;
                    pixelG = ka*pixelG + (pixelG * diffuse*lg) + specular;
                    pixelR = ka*pixelR + (pixelR * diffuse*lr) + specular;

                    if(pixelB > 255.0) pixelB = 255.0;
                    else if(pixelB < 0.0) pixelB = 0.0;

                    if(pixelG > 255.0) pixelG = 255.0;
                    else if(pixelG < 0.0) pixelG = 0.0;

                    if(pixelR > 255.0) pixelR = 255.0;
                    else if(pixelR < 0.0) pixelR = 0.0;

                    t[pos]   = (uint8) pixelB;
                    t[pos+1] = (uint8) pixelG;
                    t[pos+2] = (uint8) pixelR;
                    pos+=4;
                }
            }

        }
    delete [] nx;
    delete [] ny;
    delete [] nz;
	return t;


    }

/*
	void set_pal(unsigned char *palette, int c) {
		for(int i=0;i<c;++i) {
			outportb(0x3c8, (unsigned char)i);
			outportb(0x3c9, (unsigned char)(palette[2] >> 2  ) );
			outportb(0x3c9, (unsigned char)(palette[1] >> 2 ) );
			outportb(0x3c9, (unsigned char)(palette[0] >> 2 ) );
			palette+=3;
			}
	}
*/


/*

        void SetParameters( float X,float Y,float Z,
                            float Red,float Green,float Blue,
                            float FocusX,float FocusY,float FocusZ,
                            float Intensity,float Concentration ) {

                Surface(float **h_map, int32 w, int32 h,
                                float spec, float spec_brightness, float diff,
                                float ambient) {

*/