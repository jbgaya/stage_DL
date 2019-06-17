# Tests CUDA
Exploration de la bibliothèque mshadow.

### Description des dossiers :

- **pi-simulation** : Simple simulation de pi par méthode de Monte-Carlo sur GPU (pas de librairie particulière).

- **mshadow** : bibliothèque mshadow importée depuis https://github.com/dmlc/mshadow.

- **mshadow-sandbox** : programmes simples pour tester et comprendre la structure des objets proposés par mshadow.

- **neuralnet-MNIST** : programmes de perceptrons multicouches entraînés sur la base de données MNIST. 


### Ajouts dans la bibliothèque mshadow :
- **tensor_container.h** :
  - *ligne 189* : Ajout de TensorContainer::data(). Un accesseur-mutateur pour le tenseur contenu dans le TensorContainer. Cela permet d'utiliser **random.h** avec un Tensorcontainer en input.
