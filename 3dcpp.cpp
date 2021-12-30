#include<bits/stdc++.h>

using namespace std;

typedef struct LocType {
	int x, y, z
} Location;

typedef struct ItemType {
	int l;
	int b;
    	int h;
    	int orientation;
    	bool packed;
	Location* pos;   	
} Item;

class Container{
	int L, B, H;
	int **v;
	int **minh;
public:
	Container(int x, int y, int z){
		L=x;
		B=y;
		H=h;
		v = new int*[L];
		minh = new int*[L];
		for(int i=0; i<L; i++){
			v[i] = new int[B];
			minh[i] = new int[B];
			for(int j=0; j<B; j++){
				v[i][j] = 0;
				minh[i][j] = 0;
			}
		}	
	}	
	~Container(){
		for(int i=0; i<L; i++){
			delete[] v[i];
			delete[] minh[i];	
		}
		delete[] v;
		delete[] minh;
	}
	Location* Fit(int l, int b, int h){
		int flag = 0, x, y, base;
		Location * l = new Location;
		l->x = -1;
		l->y = -1;
		l->z = -1;
		for(x=0; x+l<=L; x++){
			for(y=0; y+b<=B; y++){
				flag = 1;
				base = v[x][y];
				if(base+h>H || base<=r)
					continue;
				for(int m=0; m<l; m++){				
					for(int n=0; n<b; n++){
						if(v[x+m][y+n]!=base || minh[x+m][y+n]>=h){
							flag=0;
							break;
						}
					}
					if(flag==0)
						break;
				}
				if(flag==1){
					l->x = x;
					l->y = y;
					l->z = base;
				}				
			}
			if(l->x>=0)
				break;
		}
		if(flag==0)
			return null;
		
		x=l->x;
		y=l->y;
		for(int m=x; m<x+l; m++)
			for(int n=y; n<y+b; n++)
				v[m][n]+=h;
		
		for(int n=y; n<y+b; n++){
			for(int m=x-1; m>=0; m++){
				if(v[m][n]>=v[x][n])
					break;
				minh[m][n] = v[x][n]-v[m][n];
			}
		}		
		return l;
	}
};


void threedcpp(vector<Item>& Items, int L, int B, int H) {
	Container C(L, B, H);
    	for(int i=Items.size()-1; i>=0; i++){
    		Item I = Items[i];
    		vector<Item> Iarr(6);
    		Iarr[0] = {I.l, I.b, I.h, 1, true, null};
    		Iarr[1] = {I.l, I.h, I.b, 2, true, null};
    		Iarr[2] = {I.b, I.l, I.h, 3, true, null};
    		Iarr[3] = {I.b, I.h, I.l, 4, true, null};
    		Iarr[4] = {I.h, I.l, I.b, 5, true, null};
    		Iarr[5] = {I.h, I.b, I.l, 6, true, null};
    		random_shuffle(Iarr.begin(), Iarr.end());
    		for(int j=0; j<6; j++){
        		// if orientation i+1 is allowed for given package then do:
        		Iarr[j].pos = C.Fit(Iarr[j].l, Iarr[j].b, Iarr[j].h)
        		if(Iarr[j].pos!=null){
        			Items[i] = Iarr[j];
        		}
    		}
    	}
}

int main(){
	// receive input data and pass on to 3dcpp fn
}
