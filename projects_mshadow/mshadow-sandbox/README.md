# Test de programmes simples utilisant mshadow
utiliser "make all" pour compiler tous les programmes. 

## Remarques sur les programmes
- **simple_allocator.cu** : Allocation d'un tableau de double dans deux tenseurs.
Il semblerait que les objets du type Tensor<gpu, 2, float> soient des sortes de pointeurs, et qu'ils ne sont là que pour donner une indication au programme sur la façon de gérer les opérations. Donc les champs important d'un Tensor sont :
  - dptr_ : pointe vers le tableau de données désiré.
  - shape_ : la dimension du tenseur.
  - stride_ : ???

- **lazy_exp.cu** : test inspiré du guide de mshadow pour bien comprendre comment marchent les *lazy expressions*. On veut faire un calcul de vecteur simple de type C = A + B. Au lieu de faire un overload d'expression classique, on "capture" l'expression souhaitée dans une structure, et cette expression ne sera évaluée qu'à la fin. L'exécution du programme donne une optimisation de l'ordre de 10^7 en gain de temps de calcul. Questions :
  - Que se passe-t-il concrètement lors du calcul avec une *lazy expression* ?
  - pourquoi le mot-clé *inline* permet un gain de temps de l'ordre de 10^1 ?
  
- **syntax.cu** : simple exploration de la syntaxe de *Tensor* dans mshadow. 
  Questions :
    - problème lors du passage de CPU à GPU. 
    - Notion de *stream* à explorer.
    
 - **defop.cu** : mapping un peu plus pousé avec des fonctions d'activation pour voir comment marchent les expression templates :
    - /!\ donner un nom particulier pour les fonctions d'activation pour ne pas entrer en conflit avec d'autres librairies. 