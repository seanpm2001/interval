# Test d'intervalles prenant en compte la précision
Deux possibilités:
* soit on impose que la précision soit exactement la meme
* soit on ne l'impose pas

Ou est utilisé == ?
* dans <=, donc ca a son importance
i<=j <=> reunion(i, j) == j
reunion prend en compte la précision: le résultat a la précision la plus fine des deux opérandes

i<=j pourrait prendre la précision en considération, et considérer que la précision de i doit etre plus grossière que celle de j (toutes les valeurs de i sont présentes dans j)

Ou est utilisé <=? 
Dans `check.cpp > analyzeUnaryMethod`, mais en conjonction avec un test sur les précisions (à l'extérieur).

**Conclusion:** Il vaut peut-etre mieux que la précision reste en-dehors des tests, quitte à la rajouter après. 

# `check.cpp > check`

Est-ce qu'il faut prendre en compute la précision ici?
C'est une fonction void et non booléenne, qui affiche le résultat, donc on a plus de nuances à notre portée.
=> Afficher un cas ou les précisions sont égales et un cas ou elles sont différentes
(plutot un amendement lorsqu'elles )

# Warnings

**Note:** Ils sont tous dans soit dans `tlib`, soit dans `check.cpp`.
Dans le premier cas, cela concerne les auteurs de la librairie.
Dans le deuxième cas, il suffit de modifier quelque chose dans ce fichier pour faire réapparaître les warnings.

## Cast float en int en sortie de truncate

### Contexte

Certaines primitives ont des comportements différents lorsqu'elles sont appliquées à des entiers. 
Ces primitives sont avant touts des fonctions binaires (Add, Mult), c'est pourquoi la fonction `analyzeBinaryMethod` présente deux branches selon que la précision des intervalles d'entrée soit `<0` (indicateur d'une opération flottante) ou `>= 0` (indicateur d'une opération entière). 

# Erreurs dans les tests

## Singletons 

La précision mesurée d'un singleton est de `INT_MAX`, car il faut au moins deux points dans un intervalle pour mesurer correctement une précision.

**Solution:** Ne pas comparer la précision lorsqu'on est sur un singleton.

# Fonctions d'intervalles à compléter

## Rem
Refaire le meme raisonnement que pour Mod, en prenant en compte les changements dûs à tie to even.

## Fonctions trigonométriques
Refaire le raisonnement des Taylor fallback.

## Probablement d'autres fonctions
Faire une passe sur toutes les primitives pour vérifier que tout est en ordre.

# Documentation
- Méthodes de test avec les différents pitfall/edge cases (singletons, ensembles vides etc)
- Backwards propagation
- Fuzzy precision