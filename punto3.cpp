#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define FILAS 200
#define COLUMNAS 200

using namespace std;

struct Orden
{
    unsigned int Espera = 0;
    bool SoltarGranada1 = false;
    bool SoltarGranada2 = false;
    bool AtaqueKamikaze = false;
    bool Aterrizaje = false;
    bool Despegue = false;
    int SiguienteX = 0;
    int SiguienteY = 0;
};

struct OrdenArchivo
{
    int X = 0;
    int Y = 0;
    unsigned int Espera = 0;
    bool SoltarGranada1 = false;
    bool SoltarGranada2 = false;
    bool AtaqueKamikaze = false;
    bool Aterrizaje = false;
    bool Despegue = false;
    int SiguienteX = 0;
    int SiguienteY = 0;
};

bool CoordenadasValidas(int X, int Y){
    return X >= 0 && X < FILAS && Y >= 0 && Y < COLUMNAS;
}

bool MovimientoValido(int X, int Y, int SigX, int SigY){
    int DiferenciaX = abs(SigX - X);
    int DiferenciaY = abs(SigY - Y);
    return DiferenciaX + DiferenciaY == 1;
}

void mostrarmenu()
{
    cout << "1. Cargar archivo de ataque en memoria." << endl;
    cout << "2. Mostrar ataque cargado." << endl;
    cout << "3. Crear un archivo de ataque nuevo." << endl;
    cout << "4. Corregir un registro del archivo. "<< endl;
    cout << "5. Corregir un registro en memoria." << endl;
    cout << "6. Guardar memoria en un archivo nuevo." << endl;
    cout << "7. Visualizar un archivo de ataque en html." << endl;
}

bool ValidarArchivo(FILE *f)
{
    OrdenArchivo Registro;
    int CantidadDespegues=0;

    fseek(f, 0, SEEK_SET);
    while (fread(&Registro, sizeof(OrdenArchivo), 1, f)==1)
    {
        if (Registro.X<0 || Registro.X>=FILAS || Registro.Y<0 || Registro.Y>=COLUMNAS) // Validar de las coordenadas.
        {
            cout << "El archivo contiene coordenadas invalidas." << endl;
            return false;
        }
        if (Registro.Despegue) // Contar los despegues.
        {
            CantidadDespegues++;
            if (Registro.SoltarGranada1 || Registro.SoltarGranada2 || Registro.AtaqueKamikaze) //Verificar que los despegues no tengan ataques asociados.
            {
                cout << "El despegue no debe tener ataques asociados." << endl;
                return false;
            }
        }
    }
    if (CantidadDespegues!=1) //Comprobar la cantidad total de despegues.
    {
        cout << "El archivo debe tener exactamente un despegue." << endl;
        return false;
    }
    return true;
}


void CargarArchivo(Orden Memoria[FILAS][COLUMNAS])
{
    OrdenArchivo Registro;
    FILE *f;
    char RutaNombreArchivo[256];

    cout << "Indique la ruta y nombre del archivo." << endl;
    cin >> RutaNombreArchivo;
    f=fopen(RutaNombreArchivo, "rb");
    if (!f)
    {
        cout << "No se púdo abrir el archivo." << endl;
        return;
    }
    else
    {
        if (ValidarArchivo(f)) // Validar el archivo antes de cargarlo.
        {
            fseek(f, 0, SEEK_SET);
            while (fread(&Registro, sizeof(OrdenArchivo), 1, f)==1)
            {
                Memoria[Registro.X][Registro.Y].Espera = Registro.Espera;
                Memoria[Registro.X][Registro.Y].SoltarGranada1 = Registro.SoltarGranada1;
                Memoria[Registro.X][Registro.Y].SoltarGranada2 = Registro.SoltarGranada2;
                Memoria[Registro.X][Registro.Y].AtaqueKamikaze = Registro.AtaqueKamikaze;
                Memoria[Registro.X][Registro.Y].Aterrizaje = Registro.Aterrizaje;
                Memoria[Registro.X][Registro.Y].Despegue = Registro.Despegue;
                Memoria[Registro.X][Registro.Y].SiguienteX = Registro.SiguienteX;
                Memoria[Registro.X][Registro.Y].SiguienteY = Registro.SiguienteY;
            }
            cout << "Lectura de archivos finalizada." << endl;
        }
        else
        {
            cout << "El archivo es invalido. No se cargo en memoria." << endl;
        }
    }
    fclose(f);
}

void MostrarAtaque()
{
    cout << " opcion 2: Mostrar ataque cargado " << endl; 
}

void CrearArchivo()
{
    FILE *f;
    char RutaNombreArchivo[256];
    char siono;
    int XActual, YActual;
    int CantidadRegistros = 0;
    bool Finalizado = false;
    bool Granada1suelta = false;
    bool Granada2suelta = false;

    cout << "Indique la ruta y nombre del archivo a crear:" << endl;
    cin >> RutaNombreArchivo;
    f=fopen(RutaNombreArchivo, "wb");
    if (!f)
    {
        cout << "No se púdo crear el archivo." << endl;
        return;
    }
    cout << "Ingrese las coordenadas iniciales (X Y) del ataque (0-199):" << endl;
    cin >> XActual >> YActual;
    if (!CoordenadasValidas(XActual, YActual))
    {
        cout << "Coordenadas iniciales invalidas." << endl;
        fclose(f);
        return;
    }

    do
    {
        OrdenArchivo Registro{};
        Registro.X = XActual;
        Registro.Y = YActual;
        cout << "las coordenadas actuales son: (" << XActual << ", " << YActual << ")" << endl;
        Registro.Despegue = (CantidadRegistros == 0);
        cout << "Desea aterrizar en esta posicion? (s/n):" << endl;
        cin >> siono;
        Registro.Aterrizaje = (siono == 's' || siono == 'S');
        if (Registro.Aterrizaje && CantidadRegistros == 0)
        {
            cout << "No se puede aterrizar en la primera posicion." << endl;
            Registro.Aterrizaje = false;
        }
        if(Registro.Aterrizaje){
            fwrite(&Registro, sizeof(OrdenArchivo), 1, f);
        if (fwrite(&Registro, sizeof(OrdenArchivo), 1, f) != 1)
        {
        cout << "Error al escribir el registro." << endl;
        fclose(f);
        return;
        }
            break;
        }
        if(!Granada1suelta){
            cout << "Desea soltar la granada 1 en esta posicion? (s/n):" << endl;
            cin >> siono;
            Registro.SoltarGranada1 = (siono == 's' || siono == 'S');
             if (Registro.SoltarGranada1 && Granada1suelta)
             {
            cout << "La granada 1 ya fue soltada previamente." << endl;
            Registro.SoltarGranada1 = false;
             }
             else if (Registro.SoltarGranada1)
            {
            Granada1suelta = true;
            }
        }else if(!Granada2suelta){
            cout << "Desea soltar la granada 2 en esta posicion? (s/n):" << endl;
            cin >> siono;
            Registro.SoltarGranada2 = (siono == 's' || siono == 'S');
             if (Registro.SoltarGranada2 && Granada2suelta)
             {
            cout << "La granada 2 ya fue soltada previamente." << endl;
            Registro.SoltarGranada2 = false;
             }
             else if (Registro.SoltarGranada2)
            {
            Granada2suelta = true;
            }
        }
        if(Registro.SoltarGranada1 && CantidadRegistros == 0){
            cout << "No se puede soltar la granada 1 en la primera posicion." << endl;
            Registro.SoltarGranada1 = false;
            Granada1suelta = false;
        }
        if(Registro.SoltarGranada2 && CantidadRegistros == 0){
            cout << "No se puede soltar la granada 2 en la primera posicion." << endl;
            Registro.SoltarGranada2 = false;
            Granada2suelta = false;
        }
        cout << "Desea realizar el ataque kamikaze? (s/n):" << endl;
        cin >> siono;
        Registro.AtaqueKamikaze = (siono == 's' || siono == 'S');
        if(Registro.AtaqueKamikaze && CantidadRegistros == 0){
            cout << "No se puede realizar el ataque kamikaze en la primera posicion." << endl;
            Registro.AtaqueKamikaze = false;
        }
        if(Registro.AtaqueKamikaze){
            Registro.Espera = 0;
        }else{
            cout << "Ingrese el tiempo de espera en esta posicion (0-255):" << endl;
            cin >> Registro.Espera;
        }
        cout << "Ingrese las coordenadas siguientes (X Y) del ataque (0-199):" << endl;
        cin >> Registro.SiguienteX >> Registro.SiguienteY;
        if (!CoordenadasValidas(Registro.SiguienteX, Registro.SiguienteY))
        {
            cout << "Coordenadas siguientes invalidas." << endl;
            continue;
        }
        if (!MovimientoValido(XActual, YActual, Registro.SiguienteX, Registro.SiguienteY))
        {
            cout << "Movimiento invalido. Debe moverse a una celda adyacente." << endl;
            continue;
        }
        fwrite(&Registro, sizeof(OrdenArchivo), 1, f);
        if (fwrite(&Registro, sizeof(OrdenArchivo), 1, f) != 1)
        {
        cout << "Error al escribir el registro." << endl;
        fclose(f);
        return;
        }
        CantidadRegistros++;
        XActual = Registro.SiguienteX;
        YActual = Registro.SiguienteY;
    } while (!Finalizado);
    

    fclose(f);

}

void CorregirArchivo()
{
   cout << " opcion 4:Corregir un registro del archivo " << endl;
}

void CorregirMemoria()
{
    cout << " opcion 5: Corregir un registro en memoria " << endl;
}

void GuardarMemoria()
{
    cout << " opcion 6: Guardar memoria en un archivo nuevo " << endl;
}

void VerHTML()
{
    cout << " opcion 7: Visualizar un archivo de ataque en html" << endl;
}

int main ()
{
    Orden Memoria[FILAS][COLUMNAS]={};
    char opcion;
    do
    {
        mostrarmenu();
        cout << "Ingrese una opcion: " << endl;
        cin >> opcion;
        switch(opcion)
        {
            case '1':
                CargarArchivo(Memoria);
            break;
            case '2':
                MostrarAtaque();
            break;
            case '3':
                CrearArchivo();
            break;
            case '4':
                CorregirArchivo();
            break;
            case '5':
                CorregirMemoria();
            break;
            case '6':
                GuardarMemoria();
            break;
            case '7':
                VerHTML();
            break;
            case '0':
                cout << "Saliendo del programa" << endl;
            break;
            default:
                cout << "Opcion inválida. Intente nuevamente." << endl;
            break;
        }
    }while(opcion!='0');
    return 0;
}
