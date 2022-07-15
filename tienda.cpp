#include "tienda.h"
#include "ui_tienda.h"

Tienda::Tienda(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Tienda)
{
    ui->setupUi(this);
    // Establecemos la lista de productos
    cargarProductos();
    // Con estos comando podemos mostramos el producto en combo
    foreach (Producto *p, m_productos){
        ui->inProducto->addItem(p->nombre());
    }
    // Configurar cabecera de la tabla
    // Declaramos todas las secciones que iran dentro de la tabla
    QStringList cabecera = {"Cantidad", "Producto", "P. unitario", "Subtotal"};
    // Definimos la cantidad de columnas que formaan parte de la tabla
    ui->outDetalle->setColumnCount(4);
    ui->outDetalle->setHorizontalHeaderLabels(cabecera);
    // Establecemos el subtotal en cero para ue no existan confusiones con los precios
    m_subtotal = 0;
}

Tienda::~Tienda()
{
    delete ui;
}

void Tienda::cargarProductos()
{

    // Recurrimos al directorio actual
    QDir actual = QDir::current();
    // Procedemoas a traspasar los datos del path al archivo
    QString archivoProductos = actual.absolutePath() + "/productos.csv";
    QFile archivo(archivoProductos);

    if (archivo.open(QIODevice::ReadOnly | QIODevice::Text)){

        bool primera = true;
        QTextStream in(&archivo);
        while (!in.atEnd()) {
            QString linea = in.readLine();
            if (primera){
                primera = false;
                continue;
            }
            QStringList datos = linea.split(";");
            QString precio = datos.at(2);
            float p = precio.toFloat();
            int id = datos.at(0).toInt();
            m_productos.append(new Producto(id, datos.at(1), p));
        }

        archivo.close();
    }else{
        QMessageBox::critical(this,"Error","La lista de productos no se pudo cargar");
    }

}

void Tienda::calcular(float stProducto)
{
    // Operaciones basicas de calculo de valores (IVA)
    m_subtotal += stProducto;
    float iva = m_subtotal * IVA / 100;
    float total = m_subtotal + iva;
    // Con este codigo podemos mostrar los valores operados en en la GUI
    ui->outSubtotal->setText("$ " + QString::number(m_subtotal, 'f', 2));
    ui->outIva->setText("$ " + QString::number(iva, 'f', 2));
    ui->outTotal->setText("$ " + QString::number(total, 'f', 2));
}

void Tienda::on_inProducto_currentIndexChanged(int index)
{
    // Extraemos el precio actual del producto
    float precio = m_productos.at(index)->precio();
    // Se muestra el precio del producto en la etiqueta mas el "$"
    //Para especificar que se esta trabajando en dolares
    ui->outPrecio->setText("$ " + QString::number(precio,'f',2));
    // Comando para limpiar el spinBox de ka Cantidad
    ui->inCantidad->setValue(0);
}


void Tienda::on_btnAgregar_released()
{
    // Validar que no se agregen productos cpn 0 cantidad
    int cantidad = ui->inCantidad->value();
    if (cantidad == 0){
        return;
    }
    // Exraemos los datos de la GUI
    int i = ui->inProducto->currentIndex();
    Producto *p = m_productos.at(i);

    /* Realizamos el calculo del subtotal del prducto
     * Tomando la cantidad y multiplicandola por su precio*/

    float subtotal = p->precio() * cantidad;

    /* Mediante este sodigo se añaden los productos a las 4 filas que habiamos pre establecido
     * Yendo del 0 al 3 estableciendo los 4 espacios necesarios*/

    int fila = ui->outDetalle->rowCount();
    ui->outDetalle->insertRow(fila);
    ui->outDetalle->setItem(fila, 0, new QTableWidgetItem(QString::number(cantidad)));
    ui->outDetalle->setItem(fila, 1, new QTableWidgetItem(p->nombre()));
    ui->outDetalle->setItem(fila, 2, new QTableWidgetItem(QString::number(p->precio(),'f',2)));
    ui->outDetalle->setItem(fila, 3, new QTableWidgetItem(QString::number(subtotal,'f',2)));

    /* Eliminamos los datos de la Cantidad y el Producto
     * para poder ingresar nuevos valores*/
    ui->inCantidad->setValue(0);
    ui->inProducto->setFocus();

    // Actualizamos el valor de los subtotales

    calcular(subtotal);

}

void Tienda::on_actionGuadar_triggered()
{
    // Abrimos el cuadro de dialogo en donde recibimos las opciones de guardar la factura
    QTextStream io;
    QString nombreArchivo = QFileDialog::getSaveFileName(this,"Guardar factura",QDir::current().absolutePath() + "/productos.csv","Archivos de calculo (*.csv)");
    QFile archivo;
    archivo.setFileName(nombreArchivo);
    archivo.open(QFile::WriteOnly | QFile::Truncate);
    if(!archivo.isOpen()){
        QMessageBox::critical(this,"ERROR","No se puede abrir el archivo");
        return;
    }
    io.setDevice(&archivo);
    int fila = ui->outDetalle->rowCount();
    int columna = ui->outDetalle->columnCount();
    QString celda;
    for(int i=0; i<fila; i++){
        for(int j=0; j<columna; j++){
            if(j != (columna-1)){
                celda = ui->outDetalle->item(i,j)->text()+";";
            }else{
                celda = ui->outDetalle->item(i,j)->text();
            }

            io << celda;
        }

        io << "\n";
    }

    QMessageBox::information(this,"Aviso","Archivo guardado con éxito");
    archivo.flush();
    archivo.close();
}

void Tienda::on_actionNuevo_triggered()
{

    while(ui->outDetalle->rowCount() > 0){
        ui->outDetalle->removeRow(0);
    }
    ui->statusbar->showMessage("Nueva hoja de cálculos",3000);

}

void Tienda::on_actionAcerca_de_triggered()
{
    acerca_de *dialog = new acerca_de(this);

    dialog->setVersion(VERSION);

    dialog->exec();
}


void Tienda::on_btnFacturar_clicked()
{
    if(ui->inCedula->displayText().isEmpty() || ui->inEmail->displayText().isEmpty() ||ui->inNombre->displayText().isEmpty()){
        QMessageBox::warning(this, "Advertencia", "Algunas secciones estan vacias");
        return;
    }else{
        Factura *dialog = new Factura(this);
        dialog->setTienda(TIENDA);
        dialog->exec();
    }
}
