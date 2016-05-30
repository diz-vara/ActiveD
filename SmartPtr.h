/////////////////////////////////////////////////////////
//SmartPtr.h - interface for template smart pointer



///Template Pointer
template <class T>
class Ptr 
{
private:
	T *pT;

public:
		Ptr<T>(): pT(0) {};
		~Ptr<T>() { };
    Ptr<T>(T t_existing) {pT = new T(t_existing); };
     //Ptr<T>& operator=(Ptr<T> &other);
     Ptr<T>& operator=(const Ptr<T> &other);
     Ptr<T>(const Ptr<T> &other);
     T& operator*() {return *pT;}
     T* operator->() const {return pT;}

     //void operator delete(void *p) {}
};

template <class T>
Ptr<T>::Ptr<T>(const Ptr<T> &other)
{
     if (&other == this) 
          return;
     

     pT = other.pT;
}

template <class T>
Ptr<T>::~Ptr<T>() 
{

};

template <class T>
Ptr<T>& Ptr<T>::operator=(const Ptr<T> &other) 
{ 

     //��������: ������������ ����
     if (other.pT == pT) return *this;
     // ������ - ����
     if (other.pT == 0) return *this;

     // ������ - ����������� �� �����
     if (other.Offs & 1) {
          //printf("\n%08x %08x %08x --->>> ",pT, other.pT,pT->ID);
          T* pTmp = (T*)(Pool+other.Offs-1);
          //other.pT = (T*)(Pool+other.Offs-1);

          //�������� ���
          if (pTmp->ID != pT->ID)  
               return *this;

          //� ��� ��� ��� ��� ����� � � ������ ��������, ��� ��� ���� ��������
          // ����������� �������� ����������� ��������� � �������� �������� ������������...
          //*pT = *other.pT;
          // ������ ��������� �������:

          //������� ����� �������� � ����� ���������
          char *pBuf = new char[sizeof(T)];
          char *pBufOther = new char[sizeof(T)];
          // �������� � ���� ��������
          memcpy(pBuf,pT,sizeof(T));

          //��������������� ��������� ������ ��� ������ �����
          ZeroMemory(pBufOther,sizeof(T));
          //������ ����� ���� �������� � ���� ����� - �� ������ �������� ������!!!
          memcpy(pBufOther,pTmp,pTmp->Sz);


          //��������� �� �����
          T* newT = (T*)pBuf;
          //� �� ����� ������ ������
          T* oldT = (T*)pBufOther;

          //����� ����������� (������������) ������������
          *pT = *oldT;
          // ������ ������� - ��������� �������
          if (pT->Sz < newT->Sz) {
               //��������������� �������� ���������� "������" ������
               memcpy(pT->GetDataAddr()+pT->GetDataSize(),
                    newT->GetDataAddr()+pT->GetDataSize(),
                    newT->GetDataSize()-pT->GetDataSize());
               //��������������� ������
               pT->Sz = newT->Sz;
          } 
          delete pBuf;
          delete pBufOther;
          //printf(">>%08x %08x %08x\n",pT, other.pT,pT->ID);
     } else {
          //������ - ������� ���������
          //�������� ���

          if (pT && other->ID != pT->ID)     
               return *this;
          delete pT;
          pT = new T;
          *pT = *other.pT;
     }
     return *this;
}
