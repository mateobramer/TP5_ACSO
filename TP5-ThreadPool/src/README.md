# Trabajo Práctico 2: Plantas Nativas 🌱


En este trabajo práctico utilizaremos las herramientas estudiadas en clase para resolver problemas relacionados con aprendizaje reforzado, aprendizaje automático, visión artificial y procesamiento de lenguaje natural.

### Formato de entrega

Debe entregarse un Jupyter Notebook por cada ejercicio. Se entregará mediante un _PR_ (pull request) a este repositorio con todos los ejercicios. El notebook debe haber sido ejecutado y estar guardado con los resultados de las ejecuciones. Debe estar dentro de una carpeta con su apellido, dentro de la carpeta `entregas` y el nombre debe ser `TP2-RL.ipynb`, `TP2-ML.ipynb`, `TP2-CV.ipynb` y `TP2-NLP.ipynb` según corresponda.

### Librerías externas
Está permitido el uso de Numpy, TQDM, Matplotlib, Seaborn, Pandas, Scikit-learn, Xgboost, Pytorch, Pytorch-lightning, Tensorflow, Keras y Gensim. Si desea utilizar alguna otra librería, consulte con los docentes.

### Checkpoint
Se debe entregar al menos **dos ejercicios funcionando** (aunque no estén terminados) para demostrar avance **antes del domingo 25 de mayo a las 23:59 hs**. Este PR debe tener como título `Checkpoint APELLIDO`. Si no se cumple con este requisito, se restará entre 0 y 1 punto de la nota final.

### Entrega final
La fecha de entrega es el **domingo 15 de Junio a las 23:59 hs**

---
## Parte 1 Aprendizaje Reforzado: Mi vivero y yo 🪴

Tenemos unas ~2000 plantas nativas que fuimos juntando a lo largo de los años. Están en un vivero que alquilamos, porque si no, no tendríamos dónde meterlas.  
Nuestro objetivo es conseguir **más plantas**. ¡Ayudanos!

![Mapa topológico del area](grafo.png)

### Reglas

#### Estado

- El estado se define como la cantidad de dinero y la ubicación en el grafo.

#### Acciones

- Las acciones disponibles consisten en moverse a un nodo vecino conectado por una arista.


#### Recompensa y transiciones
La recompensa se define en función de las plantas ganadas o perdidas:

- Empezamos en el **nodo 0**, con entre 2 y 8 pesos.
- En el **nodo 1** está el mercado de plantas que nos permite comprar 3 nativas por 5 pesos. `reward = +3`
- En el **nodo 2** está la feria botánica de la Facultad de Agronomía, donde podemos vender una plantita por 1 peso si no queda otra (nadie las quiere). `reward = -1`
- Al llegar al **nodo 3** (fin de mes), si todo sale bien, recibimos dos plantitas de regalo de amigos: `reward = +2` si tenemos más de 4 pesos para pagar el alquiler del vivero; si no, `reward = -2000` (porque perdemos todas nuestras plantas).
- Si nos quedamos sin dinero en algún momento, también  perdemos todo. `reward = -2000`

#### Factor de descuento

- El factor de descuento es `0.99`.

### Instrucciones:

1. Programe el ambiente que simule mi mes en el vivero.
2. Encuentre la política óptima usando **Value Iteration**, para esto lea el capítulo 4.4. del libro de Sutton y Barto e implemente el algoritmo. Use una tolerancia `tol = 0.1`
3. Imprima una tabla de ubicación vs dinero que muestre el valor en plantas ganadas de cada estado alcanzable.
4. Imprima una tabla de ubicación vs dinero que muestre la política óptima para cada estado alcanzable.
5. Analice la dependencia de la política óptima con respecto al factor de descuento. ¿Qué sucede si el factor de descuento es 0.1? ¿Y si es 1?

---
## Parte 2 Aprendizaje Automático: La flor secreta🪻

Recientemente se desclasificaron unos archivos de la CIA sobre una flor híbrida con propiedades desconocidas. Sabemos que está basada en algunos iris, pero no sabemos en cuáles. Ayudá al colectivo de biohackers a entrenar un modelo de clasificación sobre el dataset de iris. Luego probaremos el modelo con el dataset de la flor híbrida.

### Instrucciones:

1. Importe el dataset de desarrollo https://www.kaggle.com/code/nathsubhajit/iris-flower-classification y el dataset de la flor secreta (dentro del .zip).
2. Grafique las flores del conjunto de desarrollo en un plano usando dos características a elección. Use distintos colores para cada especie.
3. Elija un modelo de clasificación supervisada (por ejemplo, Árbol de Decisión, Random Forest, KNN, etc.) y realice una búsqueda de hiperparámetros mediante validación cruzada sobre el dataset de desarrollo con 3 folds. Puede utilizar GridSearchCV o RandomizedSearchCV.
4. Una vez finalizada la búsqueda, entrene el mejor modelo encontrado sobre todo el conjunto de desarrollo, e informe su accuracy promedio obtenida durante la validación cruzada.
5. Use el modelo entrenado para realizar predicciones sobre la flor híbrida. En un gráfico con dos características a elección, muestre las flores del dataset de desarrollo y la flor híbrida, con distintos colores por especie (e indique cuál fue la predicción).
6. Calcule los centroides (media de cada característica) para cada especie del conjunto de desarrollo, y compárelos con el centroide de la flor híbrida. ¿A qué especie o especies conocidas se parece más la flor híbrida?


---
## Parte 3 Visión Artificial: Plantificador 🌿

¡Nos contrataron para un trabajo con plantas nativas! Tenemos que proveer plantas para el nuevo edificio de innovación. Pero como ya tenemos muchas plantas, no sabemos ni qué tenemos. Necesitamos un sistema de clasificación automática para poder organizarnos.

### Instrucciones:

1. Cargue el dataset de imágenes fragmentadas.
2. Separe los datos en conjuntos de train y validation (80%/20%).
3. Entrene un modelo con la arquitectura sugerida o similar:
```
Conv2D 16 3x3 ReLU
MaxPool2D 2x2
Conv2D 32 3x3 ReLU
MaxPool2D 2x2
Conv2D 64 3x3 ReLU
MaxPool2D 2x2
Dropout 0.5
Flatten
Linear 64 ReLU
Dropout 0.5
Linear 3 Softmax
```
1. Grafique la curva de pérdida durante el entrenamiento de ambos conjuntos.
2. Informe la accuracy obtenida sobre cada conjunto.(se espera una accuracy cercana al 80% para este problema)
3. Muestre 5 parches correcatamete clasificados de cada clase y 5 incorretamente clasificados de cada clase, todos perteneciente al conjunto de validación.
4. Pruebe el modelo con 3 fotografias de estas especies, tomadas por usted en el edificio Sullair, partalas en parches del tamaño correcto (puede usar el script patch.py) y infome las probabilidades de cada parche. 
5. Explique brevemente para los parches en los que la clasificación no fue tan buena por que ocurrio esto.

---
## Parte 4: NLPlantas 🌷

Nuestro vivero lanzó una campaña online y recibió **reseñas de clientes** sobre las plantas que compraron. Algunas son muy buenas, otras no tanto... El objetivo de este ejercicio es **clasificar automáticamente si una reseña es positiva o negativa** usando herramientas de procesamiento de lenguaje natural (NLP).

Para esto, vamos a comparar **dos enfoques distintos**:

- Un modelo preentrenado BERT para análisis de sentimiento.
- Un modelo basado en embeddings clásicos (Word2Vec) y centroides.

### 📁 Archivos de datos

- `reseñas_train.csv`: contiene reseñas etiquetadas como `positiva` o `negativa` (para entrenamiento).
- `reseñas_test.csv`: contiene reseñas también etiquetadas (para testeo).

Ambos archivos tienen las siguientes columnas:

- `comentario`: el texto de la reseña.
- `etiqueta`: `"positiva"` o `"negativa"`.

### 1. Clasificación con BERT

Usaremos un modelo preentrenado para análisis de sentimientos multilingüe. Recomendamos:
nlptown/bert-base-multilingual-uncased-sentiment


Este modelo devuelve una puntuación de **1 a 5 estrellas**, donde recomendamos tomar 4 y 5 estrellas como positivo y 1,2,3 estrellas como negativo

#### Código base sugerido

```python
from transformers import pipeline

clasificador = pipeline("sentiment-analysis", model="nlptown/bert-base-multilingual-uncased-sentiment")

# Ejemplo:
print(clasificador("el living se veía bien con la planta")[0])
```
El [0] es para acceder al diccionario. Nos interesa la clave 'label'. 

Notar que en este enfoque NO usamos el conjunto de entrenamiento, ya que el modelo ya está entrenado.

Aplicá el modelo sobre los textos del reseñas_test.csv.

Calculá el accuracy, podes usar sklearn.metrics.accuracy_score.

### 2. Clasificación con Word2Vec + Centroides
Este segundo enfoque usa vectores de palabras preentrenados (Word2Vec) y un clasificador muy simple: se calcula el centroide de cada clase (promedio de vectores positivos y negativos), y se asigna a cada reseña la clase cuyo centroide esté más cerca.

📌 Pasos sugeridos
Cargá el modelo preentrenado:
```python
import gensim.downloader as api
modelo_w2v = api.load("word2vec-google-news-300")
Convertí cada texto en un vector, con algo como:

tokens = [w for w in texto.lower().split() if w in modelo_w2v]
vector = np.mean([modelo_w2v[w] for w in tokens], axis=0)
```
Calculá los centroides del conjunto de entrenamiento

Para clasificar un nuevo texto, calculá la similitud coseno con ambos centroides. Para eso vas a necesitar calcular:
```python
from sklearn.metrics.pairwise import cosine_similarity

v = texto_a_vector(texto, modelo_w2v).reshape(1, -1)
sim_pos = cosine_similarity(v, centroide_positiva.reshape(1, -1))[0][0]
sim_neg = cosine_similarity(v, centroide_negativa.reshape(1, -1))[0][0]
```
Hace la predicción a las reseñas del test y calculá el accuracy.

### 3. Comparación final
Mostrá en pantalla ambos valores de accuracy y respondé brevemente:

¿Cuál modelo funcionó mejor?

¿Por qué creés que pasa eso?

¿Qué ventajas tiene un modelo como BERT frente a Word2Vec en esta tarea?

### Librerías útiles
torch

transformers: para usar modelos preentrenados como BERT.

gensim: para cargar Word2Vec (word2vec-google-news-300).

pandas: para cargar y manejar los datasets.

numpy y scikit-learn: para vectores y métricas (cosine_similarity, accuracy_score).

IMPORTANTE: asegurate de instalar numpy<2.0 para evitar errores de compatibilidad con gensim.
