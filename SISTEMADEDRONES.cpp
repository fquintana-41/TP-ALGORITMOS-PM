#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define FILAS 200
#define COLUMNAS 200

using namespace std;

// Convencion: X = columna, Y = fila. Las matrices se indexan [Y][X] (es decir [fila][columna]).

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

void MostrarMenu()
{
    cout << "1. Cargar archivo de ataque en memoria." << endl;
    cout << "2. Mostrar ataque cargado." << endl;
    cout << "3. Crear un archivo de ataque nuevo." << endl;
    cout << "4. Corregir un registro del archivo. "<< endl;
    cout << "5. Corregir un registro en memoria." << endl;
    cout << "6. Guardar memoria en un archivo nuevo." << endl;
    cout << "7. Visualizar un archivo de ataque en html." << endl;
}

bool ValidarArchivo(FILE *Archivo)
{
    OrdenArchivo OrdenLeida; 
    OrdenArchivo OrdenActual; 
    bool CoordenadasOcupadas[FILAS][COLUMNAS]={}; 
    bool CoordenadasRecorridas[FILAS][COLUMNAS]={}; 
    long PosicionesArchivo[FILAS][COLUMNAS]; 
    int TotalDespegues=0; 
    int TotalFinalizaciones=0; 
    int TotalRegistros=0; 
    int DespegueX=0, DespegueY=0; 
    int ActualX=0, ActualY=0; 
    int TotalRecorridas=0; 

    for (int i=0; i<FILAS; i++) 
    {
        for (int j=0; j<COLUMNAS; j++)
        {
            PosicionesArchivo[i][j] = -1;
        }
    }
    fseek(Archivo, 0, SEEK_END);
    if (ftell(Archivo)==0)
    {
        cout << "El archivo esta vacio." << endl;
        return false;
    }
    fseek(Archivo, 0, SEEK_SET);
    while (true)
    {
        long PosicionRegistro = ftell(Archivo); 
        if (fread(&OrdenLeida, sizeof(OrdenArchivo), 1, Archivo)!=1)
        {
            break;
        }
        TotalRegistros++;
        if (OrdenLeida.X<0 || OrdenLeida.X>=COLUMNAS || OrdenLeida.Y<0 || OrdenLeida.Y>=FILAS) 
        {
            cout << "El archivo contiene coordenadas invalidas." << endl;
            return false;
        }
        if (CoordenadasOcupadas[OrdenLeida.Y][OrdenLeida.X]) 
        {
            cout << "El archivo contiene coordenadas repetidas." << endl;
            return false;
        }
        PosicionesArchivo[OrdenLeida.Y][OrdenLeida.X] = PosicionRegistro;
        CoordenadasOcupadas[OrdenLeida.Y][OrdenLeida.X] = true; 
        if (OrdenLeida.Despegue) 
        {
            TotalDespegues++;
            DespegueX = OrdenLeida.X;
            DespegueY = OrdenLeida.Y;
            if (OrdenLeida.SoltarGranada1 || OrdenLeida.SoltarGranada2 || OrdenLeida.AtaqueKamikaze) 
            {
                cout << "El despegue no debe tener ataques asociados." << endl;
                return false;
            }
            if (OrdenLeida.Aterrizaje)
            {
                cout << "El despegue no puede ser tambien un aterrizaje." << endl;
                return false;
            }
        }
        if (OrdenLeida.Aterrizaje) 
        {
            TotalFinalizaciones++;
        }
        if (OrdenLeida.AtaqueKamikaze) 
        {
            TotalFinalizaciones++;
        }
    }
    if (TotalDespegues!=1) 
    {
        cout << "El archivo debe tener exactamente un despegue." << endl;
        return false;
    }
    if (TotalFinalizaciones!=1)
    {
        cout << "El archivo debe tener una unica finalizacion." << endl;
        return false;
    }
    ActualX = DespegueX;
    ActualY = DespegueY;
    fseek(Archivo, PosicionesArchivo[ActualY][ActualX], SEEK_SET);
    fread(&OrdenActual, sizeof(OrdenArchivo), 1, Archivo); 
    while (true) 
    {
        TotalRecorridas++;
        if (CoordenadasRecorridas[ActualY][ActualX]) 
        {
            cout << "La ruta contiene coordenadas repetidas." << endl;
            return false;
        }
        CoordenadasRecorridas[ActualY][ActualX] = true; 
        if (OrdenActual.Aterrizaje || OrdenActual.AtaqueKamikaze)
        {
            break;
        }
        ActualX = OrdenActual.SiguienteX;
        ActualY = OrdenActual.SiguienteY;
        if (ActualX<0 || ActualX>=COLUMNAS || ActualY<0 || ActualY>=FILAS)
        {
            cout << "La ruta contiene coordenadas invalidas." << endl;
            return false;
        }
        if ((ActualX!=OrdenActual.X && ActualY!=OrdenActual.Y) || abs(ActualX - OrdenActual.X) > 1 || abs(ActualY - OrdenActual.Y) > 1) 
        {
            cout << "La ruta contiene movimientos hacia celdas no aledañas." << endl;
            return false;
        }
        if (PosicionesArchivo[ActualY][ActualX]==-1) 
        {
            cout << "La ruta contiene un hueco." << endl;
            return false;
        }
        fseek(Archivo, PosicionesArchivo[ActualY][ActualX], SEEK_SET);
        fread(&OrdenActual, sizeof(OrdenArchivo), 1, Archivo);
    }
    if (TotalRecorridas!=TotalRegistros)
    {
        cout << "La ruta no incluye todos los registros del archivo." << endl;
        return false;
    }
    return true;
}
void CargarArchivo(Orden Memoria[FILAS][COLUMNAS])
{
    OrdenArchivo OrdenLeida;
    FILE *Archivo;
    char RutaNombreArchivo[256];

    cout << "Indique el nombre del archivo." << endl;
    cin >> RutaNombreArchivo;
    strcat(RutaNombreArchivo, ".dat");
    Archivo=fopen(RutaNombreArchivo, "rb");
    if (!Archivo)
    {
        cout << "No se púdo abrir el archivo." << endl;
        return;
    }
    if (ValidarArchivo(Archivo)) 
    {
        for (int i=0; i<FILAS; i++)
        {
            for (int j=0; j<COLUMNAS; j++)
            {
                Memoria[i][j] = Orden();
            }
        }
        fseek(Archivo, 0, SEEK_SET);
        while (fread(&OrdenLeida, sizeof(OrdenArchivo), 1, Archivo)==1)
        {
            Memoria[OrdenLeida.Y][OrdenLeida.X].Espera = OrdenLeida.Espera;
            Memoria[OrdenLeida.Y][OrdenLeida.X].SoltarGranada1 = OrdenLeida.SoltarGranada1;
            Memoria[OrdenLeida.Y][OrdenLeida.X].SoltarGranada2 = OrdenLeida.SoltarGranada2;
            Memoria[OrdenLeida.Y][OrdenLeida.X].AtaqueKamikaze = OrdenLeida.AtaqueKamikaze;
            Memoria[OrdenLeida.Y][OrdenLeida.X].Aterrizaje = OrdenLeida.Aterrizaje;
            Memoria[OrdenLeida.Y][OrdenLeida.X].Despegue = OrdenLeida.Despegue;
            Memoria[OrdenLeida.Y][OrdenLeida.X].SiguienteX = OrdenLeida.SiguienteX;
            Memoria[OrdenLeida.Y][OrdenLeida.X].SiguienteY = OrdenLeida.SiguienteY;
        }
        cout << "Lectura de archivos finalizada." << endl;
    }
    else
    {
        cout << "El archivo es invalido. No se cargo en memoria." << endl;
    }
    fclose(Archivo);
}
int MarcarOcupadas(Orden Memoria[FILAS][COLUMNAS], bool Ocupadas[FILAS][COLUMNAS])
{
    int ActualX = -1;
    int ActualY = -1;
    int Total = 0;

    for (int i = 0; i < FILAS; i++)
    {
        for (int j = 0; j < COLUMNAS; j++)
        {
            Ocupadas[i][j] = false;
            if (Memoria[i][j].Despegue && ActualX == -1)
            {
                ActualX = j; // j es la columna (X).
                ActualY = i; // i es la fila (Y).
            }
        }
    }
    // Sigue la ruta desde el despegue: las casillas de paso no tienen ninguna instruccion, solo se las reconoce asi.
    while (ActualX != -1)
    {
        if (Ocupadas[ActualY][ActualX]) // La ruta se vuelve a cruzar: corta para no quedar en un bucle infinito.
        {
            break;
        }
        Ocupadas[ActualY][ActualX] = true;
        if (Memoria[ActualY][ActualX].Aterrizaje || Memoria[ActualY][ActualX].AtaqueKamikaze)
        {
            break;
        }
        int SiguienteX = Memoria[ActualY][ActualX].SiguienteX;
        int SiguienteY = Memoria[ActualY][ActualX].SiguienteY;
        if (SiguienteX < 0 || SiguienteX >= COLUMNAS || SiguienteY < 0 || SiguienteY >= FILAS)
        {
            break;
        }
        ActualX = SiguienteX;
        ActualY = SiguienteY;
    }
    // Tambien cuentan las casillas fuera de la ruta que tengan alguna instruccion o espera.
    for (int i = 0; i < FILAS; i++)
    {
        for (int j = 0; j < COLUMNAS; j++)
        {
            if (Memoria[i][j].Despegue ||
                Memoria[i][j].Espera > 0 ||
                Memoria[i][j].Aterrizaje ||
                Memoria[i][j].SoltarGranada1 ||
                Memoria[i][j].SoltarGranada2 ||
                Memoria[i][j].AtaqueKamikaze)
            {
                Ocupadas[i][j] = true;
            }
            if (Ocupadas[i][j])
            {
                Total++;
            }
        }
    }
    return Total;
}

void MostrarAtaque(Orden Memoria[FILAS][COLUMNAS])
{
    const int RegistrosPorPagina = 20; // 1 titulo + 20 registros + 1 aviso = 22 lineas: entra en una consola de 24 filas.
    bool Ocupadas[FILAS][COLUMNAS];
    int contador = 0;
    int total = 0;

    cout << " MOSTRANDO ATAQUE CARGADO EN MEMORIA " << endl;
    cin.ignore(10000, '\n'); // Descarta lo que quedo de la linea del menu para que la pausa espere un Enter de verdad.
    MarcarOcupadas(Memoria, Ocupadas);

    for (int i = 0; i < FILAS; i++)
    {
        for (int j = 0; j < COLUMNAS; j++)
        {
            if (!Ocupadas[i][j])
            {
                continue;
            }
            if (contador == RegistrosPorPagina) // La pausa va antes del registro siguiente, asi no se pide Enter de mas al final.
            {
                cout << " -- Presione Enter para continuar -- " << flush;
                cin.ignore(10000, '\n');
                contador = 0;
            }
            // Un registro por linea: (X,Y) con X = columna e Y = fila.
            cout << "(" << j << "," << i << ")"
                 << "  Esp:" << Memoria[i][j].Espera
                 << "  D:" << (Memoria[i][j].Despegue ? "Si" : "No")
                 << " G1:" << (Memoria[i][j].SoltarGranada1 ? "Si" : "No")
                 << " G2:" << (Memoria[i][j].SoltarGranada2 ? "Si" : "No")
                 << " K:" << (Memoria[i][j].AtaqueKamikaze ? "Si" : "No")
                 << " A:" << (Memoria[i][j].Aterrizaje ? "Si" : "No");
            if (!Memoria[i][j].Aterrizaje && !Memoria[i][j].AtaqueKamikaze)
            {
                cout << "  Sig:(" << Memoria[i][j].SiguienteX << "," << Memoria[i][j].SiguienteY << ")";
            }
            cout << endl;
            contador++;
            total++;
        }
    }

    if (total == 0)
    {
        cout << " No hay ningun ataque cargado en memoria todavia. " << endl;
    }
    else
    {
        cout << " Total: " << total << " registros." << endl;
    }
    cout << " Fin de la muestra del ataque. " << endl;
}

int PedirEntero(const char *Pregunta, int Minimo, int Maximo) 
{
    int Valor;
    while (true)
    {
        cout << Pregunta;
        if (cin >> Valor)
        {
            if (Valor>=Minimo && Valor<=Maximo)
            {
                return Valor;
            }
        }
        else
        {
            if (cin.eof())
            {
                cout << endl << "Entrada cerrada. Saliendo del programa." << endl;
                exit(0);
            }
            cin.clear(); 
            cin.ignore(10000, '\n');
        }
        cout << "Valor invalido. Ingrese un numero entre " << Minimo << " y " << Maximo << "." << endl;
    }
}

bool PedirSiNo(const char *Pregunta) 
{
    char Respuesta;
    while (true)
    {
        cout << Pregunta << " (s/n): ";
        if (!(cin >> Respuesta))
        {
            cout << endl << "Entrada cerrada. Saliendo del programa." << endl;
            exit(0);
        }
        if (Respuesta=='s' || Respuesta=='S')
        {
            return true;
        }
        if (Respuesta=='n' || Respuesta=='N')
        {
            return false;
        }
        cout << "Responda s o n." << endl;
    }
}

const char *SiNo(bool Valor)
{
    return Valor ? "Si" : "No";
}

void PedirDatosOrden(OrdenArchivo &NuevaOrden) 
{
    NuevaOrden.Espera = (unsigned int)PedirEntero("Espera (0 = sin espera): ", 0, 1000000);
    NuevaOrden.SoltarGranada1 = PedirSiNo("Soltar granada 1?");
    NuevaOrden.SoltarGranada2 = PedirSiNo("Soltar granada 2?");
    NuevaOrden.AtaqueKamikaze = PedirSiNo("Ataque kamikaze?");
    NuevaOrden.Aterrizaje = PedirSiNo("Aterrizaje?");
    NuevaOrden.Despegue = PedirSiNo("Despegue?");
    if (NuevaOrden.Aterrizaje || NuevaOrden.AtaqueKamikaze) 
    {
        NuevaOrden.SiguienteX = 0;
        NuevaOrden.SiguienteY = 0;
        cout << "La orden finaliza la operacion, no se pide la siguiente posicion." << endl;
    }
    else
    {
        NuevaOrden.SiguienteX = PedirEntero("Siguiente X (columna, 0-199): ", 0, COLUMNAS-1);
        NuevaOrden.SiguienteY = PedirEntero("Siguiente Y (fila, 0-199): ", 0, FILAS-1);
    }
}

void MostrarOrdenArchivo(const OrdenArchivo &OrdenAMostrar) 
{
    cout << "Posicion: (" << OrdenAMostrar.X << "," << OrdenAMostrar.Y << ")" << endl;
    cout << "Espera: " << OrdenAMostrar.Espera << endl;
    cout << "Soltar granada 1: " << SiNo(OrdenAMostrar.SoltarGranada1) << endl;
    cout << "Soltar granada 2: " << SiNo(OrdenAMostrar.SoltarGranada2) << endl;
    cout << "Ataque kamikaze: " << SiNo(OrdenAMostrar.AtaqueKamikaze) << endl;
    cout << "Aterrizaje: " << SiNo(OrdenAMostrar.Aterrizaje) << endl;
    cout << "Despegue: " << SiNo(OrdenAMostrar.Despegue) << endl;
    cout << "Siguiente posicion: (" << OrdenAMostrar.SiguienteX << "," << OrdenAMostrar.SiguienteY << ")" << endl;
}

void CrearArchivo()
{
    OrdenArchivo NuevaOrden;
    FILE *Archivo;
    char RutaNombreArchivo[256];
    static bool CoordenadasUsadas[FILAS][COLUMNAS]; 
    int TotalRegistros=0; 
    bool Continuar;

    cout << "Indique nombre del archivo a crear." << endl;
    cin >> RutaNombreArchivo;
    strcat(RutaNombreArchivo, ".dat");
    Archivo=fopen(RutaNombreArchivo, "rb"); 
    if (Archivo)
    {
        fclose(Archivo);
        if (!PedirSiNo("El archivo ya existe. Desea sobrescribirlo?"))
        {
            cout << "Operacion cancelada." << endl;
            return;
        }
    }
    Archivo=fopen(RutaNombreArchivo, "wb");
    if (!Archivo)
    {
        cout << "No se pudo crear el archivo." << endl;
        return;
    }
    for (int i=0; i<FILAS; i++) 
    {
        for (int j=0; j<COLUMNAS; j++)
        {
            CoordenadasUsadas[i][j] = false;
        }
    }
    do
    {
        NuevaOrden = OrdenArchivo(); 
        cout << endl << "Registro numero " << TotalRegistros+1 << endl;
        NuevaOrden.X = PedirEntero("Coordenada X (columna, 0-199): ", 0, COLUMNAS-1);
        NuevaOrden.Y = PedirEntero("Coordenada Y (fila, 0-199): ", 0, FILAS-1);
        if (CoordenadasUsadas[NuevaOrden.Y][NuevaOrden.X]) 
        {
            cout << "Ya se ingreso un registro en esas coordenadas." << endl;
            Continuar = true;
            continue;
        }
        PedirDatosOrden(NuevaOrden);
        if (fwrite(&NuevaOrden, sizeof(OrdenArchivo), 1, Archivo)!=1)
        {
            cout << "Error al escribir en el archivo." << endl;
            break;
        }
        CoordenadasUsadas[NuevaOrden.Y][NuevaOrden.X] = true;
        TotalRegistros++;
        Continuar = PedirSiNo("Desea ingresar otro registro?");
    }while (Continuar);
    fclose(Archivo);
    cout << "Archivo creado con " << TotalRegistros << " registros." << endl;
}

void CorregirArchivo()
{
    OrdenArchivo OrdenLeida;
    OrdenArchivo OrdenNueva;
    FILE *Archivo;
    char RutaNombreArchivo[256];
    int BuscadoX, BuscadoY; 
    long PosicionRegistro=-1; 

    cout << "Indique nombre del archivo a corregir." << endl;
    cin >> RutaNombreArchivo;
    strcat(RutaNombreArchivo, ".dat");
    Archivo=fopen(RutaNombreArchivo, "rb+");
    if (!Archivo)
    {
        cout << "No se pudo abrir el archivo." << endl;
        return;
    }
    BuscadoX = PedirEntero("Coordenada X (columna) del registro a corregir (0-199): ", 0, COLUMNAS-1);
    BuscadoY = PedirEntero("Coordenada Y (fila) del registro a corregir (0-199): ", 0, FILAS-1);
    while (fread(&OrdenLeida, sizeof(OrdenArchivo), 1, Archivo)==1) 
    {
        if (OrdenLeida.X==BuscadoX && OrdenLeida.Y==BuscadoY)
        {
            PosicionRegistro = ftell(Archivo) - (long)sizeof(OrdenArchivo);
            break;
        }
    }
    if (PosicionRegistro==-1)
    {
        cout << "No existe un registro en esas coordenadas dentro del archivo." << endl;
        fclose(Archivo);
        return;
    }
    cout << endl << "Registro actual:" << endl;
    MostrarOrdenArchivo(OrdenLeida);
    cout << endl << "Ingrese los datos nuevos (las coordenadas se mantienen)." << endl;
    OrdenNueva.X = BuscadoX;
    OrdenNueva.Y = BuscadoY;
    PedirDatosOrden(OrdenNueva);
    fseek(Archivo, PosicionRegistro, SEEK_SET); 
    if (fwrite(&OrdenNueva, sizeof(OrdenArchivo), 1, Archivo)==1)
    {
        cout << "Registro reemplazado." << endl;
    }
    else
    {
        cout << "Error al escribir en el archivo." << endl;
    }
    fclose(Archivo);
}

void CorregirMemoria(Orden Memoria[FILAS][COLUMNAS])
{
    OrdenArchivo OrdenActual; 
    OrdenArchivo OrdenNueva;
    int BuscadoX, BuscadoY; 

    BuscadoX = PedirEntero("Coordenada X (columna) del registro a corregir (0-199): ", 0, COLUMNAS-1);
    BuscadoY = PedirEntero("Coordenada Y (fila) del registro a corregir (0-199): ", 0, FILAS-1);
    OrdenActual.X = BuscadoX;
    OrdenActual.Y = BuscadoY;
    OrdenActual.Espera = Memoria[BuscadoY][BuscadoX].Espera;
    OrdenActual.SoltarGranada1 = Memoria[BuscadoY][BuscadoX].SoltarGranada1;
    OrdenActual.SoltarGranada2 = Memoria[BuscadoY][BuscadoX].SoltarGranada2;
    OrdenActual.AtaqueKamikaze = Memoria[BuscadoY][BuscadoX].AtaqueKamikaze;
    OrdenActual.Aterrizaje = Memoria[BuscadoY][BuscadoX].Aterrizaje;
    OrdenActual.Despegue = Memoria[BuscadoY][BuscadoX].Despegue;
    OrdenActual.SiguienteX = Memoria[BuscadoY][BuscadoX].SiguienteX;
    OrdenActual.SiguienteY = Memoria[BuscadoY][BuscadoX].SiguienteY;
    cout << endl << "Registro actual:" << endl;
    MostrarOrdenArchivo(OrdenActual);
    cout << endl << "Ingrese los datos nuevos (las coordenadas se mantienen)." << endl;
    OrdenNueva.X = BuscadoX;
    OrdenNueva.Y = BuscadoY;
    PedirDatosOrden(OrdenNueva);
    Memoria[BuscadoY][BuscadoX].Espera = OrdenNueva.Espera;
    Memoria[BuscadoY][BuscadoX].SoltarGranada1 = OrdenNueva.SoltarGranada1;
    Memoria[BuscadoY][BuscadoX].SoltarGranada2 = OrdenNueva.SoltarGranada2;
    Memoria[BuscadoY][BuscadoX].AtaqueKamikaze = OrdenNueva.AtaqueKamikaze;
    Memoria[BuscadoY][BuscadoX].Aterrizaje = OrdenNueva.Aterrizaje;
    Memoria[BuscadoY][BuscadoX].Despegue = OrdenNueva.Despegue;
    Memoria[BuscadoY][BuscadoX].SiguienteX = OrdenNueva.SiguienteX;
    Memoria[BuscadoY][BuscadoX].SiguienteY = OrdenNueva.SiguienteY;
    cout << "Registro reemplazado en memoria." << endl;
}

void GuardarMemoria(Orden Memoria[FILAS][COLUMNAS])
{
    OrdenArchivo Registro;
    FILE *f;
    char RutaNombreArchivo[256];
    bool Ocupadas[FILAS][COLUMNAS];
    int registrosGuardados = 0;

    cout << " GUARDAR MEMORIA EN UN ARCHIVO NUEVO " << endl;
    cout << "Indique la ruta y nombre del archivo a crear: ";
    cin >> RutaNombreArchivo;
    strcat(RutaNombreArchivo, ".dat");
    f = fopen(RutaNombreArchivo, "wb");
    if (f == NULL)
    {
        cout << "No se pudo crear o abrir el archivo." << endl;
        return;
    }
    MarcarOcupadas(Memoria, Ocupadas);
    for (int i = 0; i < FILAS; i++)
    {
        for (int j = 0; j < COLUMNAS; j++)
        {
            if (Ocupadas[i][j])
            {
                Registro.X = j; // j es la columna (X).
                Registro.Y = i; // i es la fila (Y).
                Registro.Espera = Memoria[i][j].Espera;
                Registro.SoltarGranada1 = Memoria[i][j].SoltarGranada1;
                Registro.SoltarGranada2 = Memoria[i][j].SoltarGranada2;
                Registro.AtaqueKamikaze = Memoria[i][j].AtaqueKamikaze;
                Registro.Aterrizaje = Memoria[i][j].Aterrizaje;
                Registro.Despegue = Memoria[i][j].Despegue;
                Registro.SiguienteX = Memoria[i][j].SiguienteX;
                Registro.SiguienteY = Memoria[i][j].SiguienteY;

                fwrite(&Registro, sizeof(OrdenArchivo), 1, f);
                registrosGuardados++;
            }
        }
    }

    fclose(f);
    cout << "Se guardaron " << registrosGuardados << " registros exitosamente en el archivo." << endl;
}

void VerHTML(Orden Memoria[FILAS][COLUMNAS])
{
    FILE *ArchivoHTML;
    char RutaNombreArchivo[256];
    bool CoordenadasOcupadas[FILAS][COLUMNAS] = {}; // Identifica las posiciones que contienen ordenes. Se indexa [Y][X].
    unsigned char Conexiones[FILAS][COLUMNAS] = {}; // Hacia que lados se conecta la linea de la ruta en cada casilla.
    const int Margen = 2; // Casillas extra que se muestran alrededor de la zona ocupada.
    const unsigned char ARRIBA=1, ABAJO=2, IZQUIERDA=4, DERECHA=8;
    int DespegueX=-1;
    int DespegueY=-1;
    int ActualX, ActualY;
    int SiguienteX, SiguienteY;
    int MinX, MaxX, MinY, MaxY; // Limites de la zona ocupada por la ruta.
    int DesdeColumna, HastaColumna, DesdeFila, HastaFila; // Limites de la zona a mostrar.

    for (int Fila=0; Fila<FILAS; Fila++) // Busca el despegue para encontrar el inicio del recorrido.
    {
        for (int Columna=0; Columna<COLUMNAS; Columna++)
        {
            if (Memoria[Fila][Columna].Despegue)
            {
                DespegueX=Columna;
                DespegueY=Fila;
                break;
            }
        }

        if (DespegueX!=-1)
        {
            break;
        }
    }

    if (DespegueX==-1) 
    {
        cout << "No hay ningun ataque cargado en memoria." << endl;
        return;
    }
    // Recorre la ruta para identificar las posiciones ocupadas, las conexiones y los limites de la zona.
    ActualX = DespegueX;
    ActualY = DespegueY;
    MinX = MaxX = DespegueX;
    MinY = MaxY = DespegueY;
    while (true)
    {
        CoordenadasOcupadas[ActualY][ActualX]=true;

        if (ActualX<MinX) MinX=ActualX;
        if (ActualX>MaxX) MaxX=ActualX;
        if (ActualY<MinY) MinY=ActualY;
        if (ActualY>MaxY) MaxY=ActualY;

        if (Memoria[ActualY][ActualX].Aterrizaje || Memoria[ActualY][ActualX].AtaqueKamikaze)
        {
            break;
        }

        SiguienteX = Memoria[ActualY][ActualX].SiguienteX;
        SiguienteY = Memoria[ActualY][ActualX].SiguienteY;

        // Marca de que lado sale la linea en la casilla actual y de que lado entra en la siguiente.
        if (SiguienteY==ActualY && SiguienteX==ActualX+1)
        {
            Conexiones[ActualY][ActualX] |= DERECHA;
            Conexiones[SiguienteY][SiguienteX] |= IZQUIERDA;
        }
        else if (SiguienteY==ActualY && SiguienteX==ActualX-1)
        {
            Conexiones[ActualY][ActualX] |= IZQUIERDA;
            Conexiones[SiguienteY][SiguienteX] |= DERECHA;
        }
        else if (SiguienteX==ActualX && SiguienteY==ActualY+1)
        {
            Conexiones[ActualY][ActualX] |= ABAJO;
            Conexiones[SiguienteY][SiguienteX] |= ARRIBA;
        }
        else if (SiguienteX==ActualX && SiguienteY==ActualY-1)
        {
            Conexiones[ActualY][ActualX] |= ARRIBA;
            Conexiones[SiguienteY][SiguienteX] |= ABAJO;
        }

        ActualX = SiguienteX;
        ActualY = SiguienteY;
    }

    // Zona a mostrar: la zona ocupada mas el margen, sin salirse de la grilla.
    DesdeColumna = MinX - Margen;
    HastaColumna = MaxX + Margen;
    DesdeFila = MinY - Margen;
    HastaFila = MaxY + Margen;
    if (DesdeColumna<0) DesdeColumna=0;
    if (HastaColumna>COLUMNAS-1) HastaColumna=COLUMNAS-1;
    if (DesdeFila<0) DesdeFila=0;
    if (HastaFila>FILAS-1) HastaFila=FILAS-1;

    cout << "Indique el nombre del archivo HTML." << endl;
    cin >> RutaNombreArchivo;
    strcat(RutaNombreArchivo, ".html");
    ArchivoHTML=fopen(RutaNombreArchivo, "w");

    if (ArchivoHTML==NULL)
    {
        cout << "No se pudo crear el archivo HTML." << endl;
        return;
    }

    fprintf(ArchivoHTML, "<html>\n");
    fprintf(ArchivoHTML, "<head>\n");
    fprintf(ArchivoHTML, "<meta charset=\"UTF-8\">\n");
    fprintf(ArchivoHTML, "<title>Visualizacion del ataque</title>\n");

    fprintf(ArchivoHTML, "<style>\n");

    fprintf(ArchivoHTML, "body {\n");
    fprintf(ArchivoHTML, "    font-family: Arial, sans-serif;\n");
    fprintf(ArchivoHTML, "    margin: 20px;\n");
    fprintf(ArchivoHTML, "}\n");

    // La tabla se centra con margenes automaticos.
    fprintf(ArchivoHTML, "#Tabla table {\n");
    fprintf(ArchivoHTML, "    border-collapse: collapse;\n");
    fprintf(ArchivoHTML, "    table-layout: fixed;\n");
    fprintf(ArchivoHTML, "    margin: 0 auto;\n");
    // Ancho = columnas mostradas * 75px + 40px del encabezado lateral.
    fprintf(ArchivoHTML, "    width: %dpx;\n", (HastaColumna-DesdeColumna+1)*75 + 40);
    fprintf(ArchivoHTML, "}\n");

    // position: relative permite ubicar la linea y los simbolos dentro de cada casilla.
    fprintf(ArchivoHTML, "#Tabla td {\n");
    fprintf(ArchivoHTML, "    position: relative;\n");
    fprintf(ArchivoHTML, "    border: 1px solid #BBB;\n");
    fprintf(ArchivoHTML, "    width: 75px;\n");
    fprintf(ArchivoHTML, "    height: 65px;\n");
    fprintf(ArchivoHTML, "    padding: 3px;\n");
    fprintf(ArchivoHTML, "    vertical-align: middle;\n");
    fprintf(ArchivoHTML, "    text-align: center;\n");
    fprintf(ArchivoHTML, "    overflow-wrap: anywhere;\n");
    fprintf(ArchivoHTML, "    font-size: 13px;\n");
    fprintf(ArchivoHTML, "}\n");

    // Estilo de los encabezados de filas y columnas.
    fprintf(ArchivoHTML, "#Tabla th {\n");
    fprintf(ArchivoHTML, "    border: 1px solid #BBB;\n");
    fprintf(ArchivoHTML, "    width: 75px;\n");
    fprintf(ArchivoHTML, "    padding: 3px;\n");
    fprintf(ArchivoHTML, "    text-align: center;\n");
    fprintf(ArchivoHTML, "    font-size: 12px;\n");
    fprintf(ArchivoHTML, "    font-weight: normal;\n");
    fprintf(ArchivoHTML, "    background-color: #F2F2F2;\n");
    fprintf(ArchivoHTML, "}\n");

    // Estilo del encabezado lateral de las filas.
    fprintf(ArchivoHTML, "#Tabla th:first-child {\n");
    fprintf(ArchivoHTML, "    width: 40px;\n");
    fprintf(ArchivoHTML, "}\n");

    // Estilo del titulo y del texto del despegue (centrados).
    fprintf(ArchivoHTML, "h1 {\n");
    fprintf(ArchivoHTML, "    text-align: center;\n");
    fprintf(ArchivoHTML, "    margin-bottom: 10px;\n");
    fprintf(ArchivoHTML, "    padding-bottom: 10px;\n");
    fprintf(ArchivoHTML, "    border-bottom: 1px solid #BBB;\n");
    fprintf(ArchivoHTML, "}\n");

    fprintf(ArchivoHTML, "p {\n");
    fprintf(ArchivoHTML, "    text-align: center;\n");
    fprintf(ArchivoHTML, "    margin-top: 0;\n");
    fprintf(ArchivoHTML, "    margin-bottom: 15px;\n");
    fprintf(ArchivoHTML, "}\n");

    // Estilos de la linea de la ruta (verde) y sus cuatro direcciones.
    fputs(".Barra { position: absolute; background: #2E9E4F; z-index: 1; }\n", ArchivoHTML);
    fputs(".Arriba { left: 50%; top: 0; width: 6px; height: 50%; margin-left: -3px; }\n", ArchivoHTML);
    fputs(".Abajo { left: 50%; bottom: 0; width: 6px; height: 50%; margin-left: -3px; }\n", ArchivoHTML);
    fputs(".Izquierda { top: 50%; left: 0; height: 6px; width: 50%; margin-top: -3px; }\n", ArchivoHTML);
    fputs(".Derecha { top: 50%; right: 0; height: 6px; width: 50%; margin-top: -3px; }\n", ArchivoHTML);
    fputs(".Nodo { position: absolute; left: 50%; top: 50%; width: 6px; height: 6px; margin: -3px 0 0 -3px; background: #2E9E4F; z-index: 1; }\n", ArchivoHTML);

    // Estilos de los simbolos.
    fputs(".Simbolos { position: absolute; left: 50%; top: 50%; transform: translate(-50%, -50%); display: flex; gap: 2px; z-index: 2; }\n", ArchivoHTML);
    fputs(".Cuad { display: inline-block; width: 20px; height: 20px; vertical-align: middle; }\n", ArchivoHTML);
    fputs(".Verde { background: #2E9E4F; }\n", ArchivoHTML);
    fputs(".Rojo { background: #E02424; }\n", ArchivoHTML);
    fputs(".Azul { background: #1F5FE0; }\n", ArchivoHTML);
    fputs(".Circ { display: flex; align-items: center; justify-content: center; width: 20px; height: 20px; box-sizing: border-box; border-radius: 50%; background: #FFD21F; border: 2px solid #B8960C; font-size: 11px; font-weight: bold; color: #000; }\n", ArchivoHTML);

    // Estilos de los textos de cada casilla.
    fputs(".Coord { position: absolute; top: 2px; left: 3px; font-size: 10px; color: #555; z-index: 3; }\n", ArchivoHTML);
    fputs(".Espera { position: absolute; bottom: 2px; right: 3px; font-size: 10px; color: #555; z-index: 3; }\n", ArchivoHTML);

    // Estilos de la tabla de referencias.
    fputs("#Leyenda { margin: 25px auto 0 auto; border-collapse: collapse; font-size: 14px; }\n", ArchivoHTML);
    fputs("#Leyenda th { border: 1px solid #BBB; padding: 6px 10px; background: #F2F2F2; }\n", ArchivoHTML);
    fputs("#Leyenda td { border: 1px solid #BBB; padding: 6px 10px; }\n", ArchivoHTML);
    fputs("#Leyenda td:first-child { width: 70px; text-align: center; }\n", ArchivoHTML);
    fputs(".LeyLinea { display: inline-block; width: 40px; height: 6px; background: #2E9E4F; vertical-align: middle; }\n", ArchivoHTML);
    fputs(".LeyCirculo { display: inline-flex; vertical-align: middle; }\n", ArchivoHTML);

    fprintf(ArchivoHTML, "</style>\n");
    fprintf(ArchivoHTML, "</head>\n");
    fprintf(ArchivoHTML, "<body>\n");
    fprintf(ArchivoHTML, "<h1>Visualizacion del ataque</h1>\n");

    fprintf(ArchivoHTML, "<p><strong>Despegue:</strong> (%d,%d)</p>\n", DespegueX, DespegueY);// Muestra las coordenadas del despegue (X,Y).
    fprintf(ArchivoHTML, "<div id=\"Tabla\" style=\"overflow: auto;\">\n");
    fprintf(ArchivoHTML, "<table border=\"1\">\n");

    // Genera la numeracion de las columnas mostradas.
    fprintf(ArchivoHTML, "<tr>\n");
    fprintf(ArchivoHTML, "<th></th>\n");
    for (int Columna=DesdeColumna; Columna<=HastaColumna; Columna++)
    {
        fprintf(ArchivoHTML, "<th>%d</th>\n", Columna);
    }
    fprintf(ArchivoHTML, "</tr>\n");
    for (int Fila=DesdeFila; Fila<=HastaFila; Fila++) // Genera las filas mostradas y su numeracion.
    {
        fprintf(ArchivoHTML, "<tr>\n");
        fprintf(ArchivoHTML, "<th>%d</th>\n", Fila); // Numero de la fila.
        for (int Columna=DesdeColumna; Columna<=HastaColumna; Columna++)
        {
            fprintf(ArchivoHTML, "<td>");
            if (CoordenadasOcupadas[Fila][Columna])
            {
                // Coordenadas de la orden como (X,Y) = (columna,fila).
                fprintf(ArchivoHTML, "<div class=\"Coord\">(%d,%d)</div>", Columna, Fila);

                // Linea verde de la ruta: un tramo por cada lado conectado y un nodo central.
                if (Conexiones[Fila][Columna] & ARRIBA)
                {
                    fprintf(ArchivoHTML, "<div class=\"Barra Arriba\"></div>");
                }
                if (Conexiones[Fila][Columna] & ABAJO)
                {
                    fprintf(ArchivoHTML, "<div class=\"Barra Abajo\"></div>");
                }
                if (Conexiones[Fila][Columna] & IZQUIERDA)
                {
                    fprintf(ArchivoHTML, "<div class=\"Barra Izquierda\"></div>");
                }
                if (Conexiones[Fila][Columna] & DERECHA)
                {
                    fprintf(ArchivoHTML, "<div class=\"Barra Derecha\"></div>");
                }
                fprintf(ArchivoHTML, "<div class=\"Nodo\"></div>");

                // Simbolos de la orden.
                if (Memoria[Fila][Columna].Despegue ||
                    Memoria[Fila][Columna].SoltarGranada1 ||
                    Memoria[Fila][Columna].SoltarGranada2 ||
                    Memoria[Fila][Columna].AtaqueKamikaze ||
                    Memoria[Fila][Columna].Aterrizaje)
                {
                    fprintf(ArchivoHTML, "<div class=\"Simbolos\">");
                    if (Memoria[Fila][Columna].Despegue)
                    {
                        fprintf(ArchivoHTML, "<div class=\"Cuad Verde\"></div>");
                    }
                    if (Memoria[Fila][Columna].SoltarGranada1)
                    {
                        fprintf(ArchivoHTML, "<div class=\"Circ\">1</div>");
                    }
                    if (Memoria[Fila][Columna].SoltarGranada2)
                    {
                        fprintf(ArchivoHTML, "<div class=\"Circ\">2</div>");
                    }
                    if (Memoria[Fila][Columna].AtaqueKamikaze)
                    {
                        fprintf(ArchivoHTML, "<div class=\"Cuad Rojo\"></div>");
                    }
                    if (Memoria[Fila][Columna].Aterrizaje)
                    {
                        fprintf(ArchivoHTML, "<div class=\"Cuad Azul\"></div>");
                    }
                    fprintf(ArchivoHTML, "</div>");
                }

                if (Memoria[Fila][Columna].Espera > 0)
                {
                    fprintf(ArchivoHTML, "<div class=\"Espera\">E:%u</div>", Memoria[Fila][Columna].Espera);
                }
            }
            else
            {
                fprintf(ArchivoHTML, "&nbsp;");
            }
            fprintf(ArchivoHTML, "</td>\n");
        }
        fprintf(ArchivoHTML, "</tr>\n");
    }
    fprintf(ArchivoHTML, "</table>\n");
    fprintf(ArchivoHTML, "</div>\n");

    // Tabla de referencias de los simbolos.
    fputs("<table id=\"Leyenda\">\n", ArchivoHTML);
    fputs("<tr><th>Simbolo</th><th>Significado</th></tr>\n", ArchivoHTML);
    fputs("<tr><td><div class=\"LeyLinea\"></div></td><td>Ruta del dron</td></tr>\n", ArchivoHTML);
    fputs("<tr><td><div class=\"Cuad Verde\"></div></td><td>Despegue</td></tr>\n", ArchivoHTML);
    fputs("<tr><td><div class=\"LeyCirculo\"><div class=\"Circ\">1</div></div> <div class=\"LeyCirculo\"><div class=\"Circ\">2</div></div></td><td>Suelta granada (el numero indica si es la granada 1 o la 2)</td></tr>\n", ArchivoHTML);
    fputs("<tr><td><div class=\"Cuad Rojo\"></div></td><td>Ataque kamikaze</td></tr>\n", ArchivoHTML);
    fputs("<tr><td><div class=\"Cuad Azul\"></div></td><td>Aterrizaje</td></tr>\n", ArchivoHTML);
    fputs("<tr><td>(X,Y)</td><td>Coordenadas de la casilla: X = columna, Y = fila</td></tr>\n", ArchivoHTML);
    fputs("<tr><td>E:n</td><td>Espera de la orden</td></tr>\n", ArchivoHTML);
    fputs("</table>\n", ArchivoHTML);

    fprintf(ArchivoHTML, "</body>\n");
    fprintf(ArchivoHTML, "</html>\n");
    fclose(ArchivoHTML);
    cout << "Archivo HTML creado. Abriendolo..." << endl;
    char Comando[300];
#ifdef _WIN32
    sprintf(Comando, "start \"\" \"%s\"", RutaNombreArchivo);
#elif __APPLE__
    sprintf(Comando, "open \"%s\"", RutaNombreArchivo);
#else
    sprintf(Comando, "xdg-open \"%s\" > /dev/null 2>&1 &", RutaNombreArchivo);
#endif
    system(Comando);
}

int main ()
{
    Orden Memoria[FILAS][COLUMNAS]={};
    char opcion;
    do
    {
        MostrarMenu();
        do{
            cout << "Ingrese una opcion: " << endl;
            cin >> opcion;
        } while(opcion<'0' || opcion>'7');
        switch(opcion)
        {
            case '1':
                CargarArchivo(Memoria);
            break;
            case '2':
                MostrarAtaque(Memoria);
            break;
            case '3':
                CrearArchivo();
            break;
            case '4':
                CorregirArchivo();
            break;
            case '5':
                CorregirMemoria(Memoria);
            break;
            case '6':
                GuardarMemoria(Memoria);
            break;
            case '7':
                VerHTML(Memoria);
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