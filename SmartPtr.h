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

     //проверки: присваивание себя
     if (other.pT == pT) return *this;
     // справа - ноль
     if (other.pT == 0) return *this;

     // справа - прочитанное из файла
     if (other.Offs & 1) {
          //printf("\n%08x %08x %08x --->>> ",pT, other.pT,pT->ID);
          T* pTmp = (T*)(Pool+other.Offs-1);
          //other.pT = (T*)(Pool+other.Offs-1);

          //неверный тип
          if (pTmp->ID != pT->ID)  
               return *this;

          //а вот как раз это место я и должен заменить, так как хочу избежать
          // копирования оффсетов прочитанной структуры в реальные смещения существующей...
          //*pT = *other.pT;
          // меняем следующим образом:

          //создаем буфер размером с новую структуру
          char *pBuf = new char[sizeof(T)];
          char *pBufOther = new char[sizeof(T)];
          // копируем в него исходную
          memcpy(pBuf,pT,sizeof(T));

          //предварительное обнуление памяти для правой части
          ZeroMemory(pBufOther,sizeof(T));
          //правую часть тоже копируем в свой буфер - но только реальные данные!!!
          memcpy(pBufOther,pTmp,pTmp->Sz);


          //указатель на копию
          T* newT = (T*)pBuf;
          //и на копию старых данных
          T* oldT = (T*)pBufOther;

          //вызов дефолтового (рекурсивного) присваивания
          *pT = *oldT;
          // особый интерес - изменение размера
          if (pT->Sz < newT->Sz) {
               //восстанавливаем исходное содержимое "лишних" клеток
               memcpy(pT->GetDataAddr()+pT->GetDataSize(),
                    newT->GetDataAddr()+pT->GetDataSize(),
                    newT->GetDataSize()-pT->GetDataSize());
               //восстанавливаем размер
               pT->Sz = newT->Sz;
          } 
          delete pBuf;
          delete pBufOther;
          //printf(">>%08x %08x %08x\n",pT, other.pT,pT->ID);
     } else {
          //справа - готовый указатель
          //неверный тип

          if (pT && other->ID != pT->ID)     
               return *this;
          delete pT;
          pT = new T;
          *pT = *other.pT;
     }
     return *this;
}
