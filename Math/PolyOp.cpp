#pragma GCC optimize(3)
#include<bits/stdc++.h>
#define MAXN 100005
#define INF 1000000000
#define MOD 998244353
#define F first
#define S second
using namespace std;
typedef long long ll;
typedef pair<int,int> P;
int n,k,a[MAXN];
int pow_mod(int a,int i,int m)
{
    int s=1;
    while(i)
    {
        if(i&1)  s=1LL*s*a%m;
        a=1LL*a*a%m;
        i>>=1;
    }
    return s;
}
inline int inc(int a,int b) {a+=b; return a>=MOD?a-MOD:a;}
inline int dec(int a,int b) {a-=b; return a<0?a+MOD:a;}
int Tonelli_Shanks(int n,ll p)
{
    if(p==2) return (n&1)?1:-1;
    if(pow_mod(n,p>>1,p)!=1) return -1;
    if(p&2) return pow_mod(n,(p+1)>>2,p);
    int s=__builtin_ctzll(p^1);
    int q=p>>s,z=2;
    for(;pow_mod(z,p>>1,p)==1;++z);
    int c=pow_mod(z,q,p),r=pow_mod(n,(q+1)>>1,p),t=pow_mod(n,q,p),tmp;
    for(int m=s,i;t!=1;)
    {
        for(i=0,tmp=t;tmp!=1;++i) tmp=tmp*tmp%p;
        for(;i<--m;) c=c*c%p;
        r=r*c%p;c=c*c%p;t=t*c%p;
    }
    return r;
}
namespace fft
{
    struct num
    {
        double x,y;
        num() {x=y=0;}
        num(double x,double y):x(x),y(y){}
    };
    inline num operator+(num a,num b) {return num(a.x+b.x,a.y+b.y);}
    inline num operator-(num a,num b) {return num(a.x-b.x,a.y-b.y);}
    inline num operator*(num a,num b) {return num(a.x*b.x-a.y*b.y,a.x*b.y+a.y*b.x);}
    inline num conj(num a) {return num(a.x,-a.y);}

    int base=1;
    vector<num> roots={{0,0},{1,0}};
    vector<int> rev={0,1};
    const double PI=acosl(-1.0);

    void ensure_base(int nbase)
    {
        if(nbase<=base) return;
        rev.resize(1<<nbase);
        for(int i=0;i<(1<<nbase);i++)
            rev[i]=(rev[i>>1]>>1)+((i&1)<<(nbase-1));
        roots.resize(1<<nbase);
        while(base<nbase)
        {
            double angle=2*PI/(1<<(base+1));
            for(int i=1<<(base-1);i<(1<<base);i++)
            {
                roots[i<<1]=roots[i];
                double angle_i=angle*(2*i+1-(1<<base));
                roots[(i<<1)+1]=num(cos(angle_i),sin(angle_i));
            }
            base++;
        }
    }

    void fft(vector<num> &a,int n=-1)
    {
        if(n==-1) n=a.size();
        assert((n&(n-1))==0);
        int zeros=__builtin_ctz(n);
        ensure_base(zeros);
        int shift=base-zeros;
        for(int i=0;i<n;i++)
            if(i<(rev[i]>>shift))
                swap(a[i],a[rev[i]>>shift]);
        for(int k=1;k<n;k<<=1)
        {
            for(int i=0;i<n;i+=2*k)
            {
                for(int j=0;j<k;j++)
                {
                    num z=a[i+j+k]*roots[j+k];
                    a[i+j+k]=a[i+j]-z;
                    a[i+j]=a[i+j]+z;
                }
            }
        }
    }

    vector<num> fa,fb;
    
    vector<int> multiply(vector<int> &a, vector<int> &b)
    {
        int need=a.size()+b.size()-1;
        int nbase=0;
        while((1<<nbase)<need) nbase++;
        ensure_base(nbase);
        int sz=1<<nbase;
        if(sz>(int)fa.size()) fa.resize(sz);
        for(int i=0;i<sz;i++)
        {
            int x=(i<(int)a.size()?a[i]:0);
            int y=(i<(int)b.size()?b[i]:0);
            fa[i]=num(x,y);
        }
        fft(fa,sz);
        num r(0,-0.25/sz);
        for(int i=0;i<=(sz>>1);i++)
        {
            int j=(sz-i)&(sz-1);
            num z=(fa[j]*fa[j]-conj(fa[i]*fa[i]))*r;
            if(i!=j) fa[j]=(fa[i]*fa[i]-conj(fa[j]*fa[j]))*r;
            fa[i]=z;
        }
        fft(fa,sz);
        vector<int> res(need);
        for(int i=0;i<need;i++) res[i]=fa[i].x+0.5;
        return res;
    }

    vector<int> multiply_mod(vector<int> &a,vector<int> &b,int m,int eq=0)
    {
        int need=a.size()+b.size()-1;
        int nbase=0;
        while((1<<nbase)<need) nbase++;
        ensure_base(nbase);
        int sz=1<<nbase;
        if(sz>(int)fa.size()) fa.resize(sz);
        for(int i=0;i<(int)a.size();i++)
        {
            int x=(a[i]%m+m)%m;
            fa[i]=num(x&((1<<15)-1),x>>15);
        }
        fill(fa.begin()+a.size(),fa.begin()+sz,num{0,0});
        fft(fa,sz);
        if(sz>(int)fb.size()) fb.resize(sz);
        if(eq) copy(fa.begin(),fa.begin()+sz,fb.begin());
        else
        {
            for(int i=0;i<(int)b.size();i++)
            {
                int x=(b[i]%m+m)%m;
                fb[i]=num(x&((1<<15)-1),x>>15);
            }
            fill(fb.begin()+b.size(),fb.begin()+sz,num{0,0});
            fft(fb,sz);
        }
        double ratio=0.25/sz;
        num r2(0,-1),r3(ratio,0),r4(0,-ratio),r5(0,1);
        for(int i=0;i<=(sz>>1);i++)
        {
            int j=(sz-i)&(sz-1);
            num a1=(fa[i]+conj(fa[j]));
            num a2=(fa[i]-conj(fa[j]))*r2;
            num b1=(fb[i]+conj(fb[j]))*r3;
            num b2=(fb[i]-conj(fb[j]))*r4;
            if(i!=j)
            {
                num c1=(fa[j]+conj(fa[i]));
                num c2=(fa[j]-conj(fa[i]))*r2;
                num d1=(fb[j]+conj(fb[i]))*r3;
                num d2=(fb[j]-conj(fb[i]))*r4;
                fa[i]=c1*d1+c2*d2*r5;
                fb[i]=c1*d2+c2*d1;
            }
            fa[j]=a1*b1+a2*b2*r5;
            fb[j]=a1*b2+a2*b1;
        }
        fft(fa,sz);fft(fb,sz);
        vector<int> res(need);
        for(int i=0;i<need;i++)
        {
            ll aa=fa[i].x+0.5;
            ll bb=fb[i].x+0.5;
            ll cc=fa[i].y+0.5;
            res[i]=(aa+((bb%m)<<15)+((cc%m)<<30))%m;
        }
        return res;
    }
    vector<int> square_mod(vector<int> &a,int m)
    {
        return multiply_mod(a,a,m,1);
    }
};
namespace poly
{
    int inv(int x) {return pow_mod(x,MOD-2,MOD);}
    vector<int> fa,fb,fc,fd;
    vector<int> get_inv(vector<int> &a,int n)
    {
        assert(a[0]!=0);
        if(n==1)
        {
            fa.resize(1);
            fa[0]=inv(a[0]);
            return fa;
        }
        fa=get_inv(a,(n+1)>>1);
        fb=fft::multiply_mod(fa,fa,MOD,1);
        fb=fft::multiply_mod(fb,a,MOD);
        fa.resize(n);
        for(int i=0;i<n;i++)
        {
            fa[i]=inc(fa[i],fa[i]);
            fa[i]=dec(fa[i],fb[i]);
        }
        return fa;
    }
    vector<int> get_sqrt(vector<int> &a,int n)
    {
        if(n==1) 
        {
            fc.resize(1);
            int x=Tonelli_Shanks(a[0],MOD);
            assert(x!=-1);
            fc[0]=x;return fc;
        }
        fd=get_sqrt(a,(n+1)>>1);
        fc=get_inv(fd,n);
        fd=fft::multiply_mod(fd,fd,MOD,1);
        fd.resize(n);
        for(int i=0;i<n;i++) fc[i]=1LL*fc[i]*((MOD+1)/2)%MOD;
        for(int i=0;i<n;i++) fd[i]=inc(fd[i],a[i]);
        fd=fft::multiply_mod(fd,fc,MOD);
        fd.resize(n);return fd;
    }
    vector<int> diff(vector<int> &a)
    {
        for(int i=1;i<(int)a.size();i++) a[i-1]=1LL*a[i]*i%MOD;
        if(a.size()>=1) a.resize((int)a.size()-1);
        return a;
    }
    vector<int> intg(vector<int> &a)
    {
        int sz=(int)a.size();
        a.resize(sz+1);
        static vector<int> Inv(sz+1);
        Inv[1]=1;
        for(int i=2;i<=sz;i++) Inv[i]=dec(MOD,1LL*Inv[MOD%i]*(MOD/i)%MOD);
        for(int i=sz;i>=1;i--) a[i]=1LL*a[i-1]*Inv[i]%MOD;
        a[0]=0;
        return a;
    }
};
int main()
{
    vector<int> res(20);
    res[0]=1;res[1]=2;res[2]=1;
    res=poly::get_sqrt(res,6);
    for(int i=0;i<(int)res.size();i++) printf("%d ",res[i]);
    return 0;
}

