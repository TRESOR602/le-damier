#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>
#include<stdbool.h>
#include<stddef.h>
#include<stdio.h>
#include<stdlib.h>

// Constantes du jeu
#define TAILLE 8 // Taille du damier (8x8)
#define TAILLE_CASE 80 // Taille d'une case en pixels
#define LARGEUR_FENETRE (TAILLE * TAILLE_CASE)
#define HAUTEUR_FENETRE (TAILLE * TAILLE_CASE)
// Définition des pièces
/*#define VIDE 0 
#define PION_BLANC 1
#define PION_NOIR 2
#define DAME_BLANCHE 3
#define DAME_NOIRE 4*/
typedef enum { VIDE, PION_BLANC, PION_NOIR, DAME_BLANCHE, DAME_NOIRE } TypeCase;




typedef struct {
    TypeCase type;  // Type de la case (vide, pion, dame)
} Case;

typedef struct {
    Case cases[8][8];  // Damier 8x8
} Damier;




// Structure pour représenter une position sur le damier
typedef struct {
int ligne;
int colonne;
} Position;
// Structure pour représenter un mouvement
typedef struct {
Position debut;
Position fin;
} Mouvement;
// Structure pour le jeu
typedef struct {
int plateau[TAILLE][TAILLE]; // Représentation du plateau de jeu
int joueurActuel; // 1 pour blanc, 2 pour noir
bool partieTerminee; // Indique si la partie est terminée
Position pieceSel; // Position de la pièce sélectionnée (-1,-1 si aucune)
} JeuDeDames;
// Structure pour les ressources SDL
typedef struct {
SDL_Window *fenetre; // Fenêtre principale
SDL_Renderer *rendu; // Renderer pour dessiner
TTF_Font *police; // Police pour le texte
SDL_Texture *texturePionBlanc;
SDL_Texture *texturePionNoir;
SDL_Texture *textureDameBlanc;
SDL_Texture *textureDameNoir;
} RessourcesSDL;



void initialiserDamier(Damier *damier) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if ((i + j) % 2 == 0) {
                damier->cases[i][j].type = VIDE;  // Les cases blanches sont vides
            } else {
                if (i < 3) {
                    damier->cases[i][j].type = PION_NOIR;  // Pions noirs sur les 3 premières lignes
                } else if (i > 4) {
                    damier->cases[i][j].type = PION_BLANC;  // Pions blancs sur les 3 dernières lignes
                } else {
                    damier->cases[i][j].type = VIDE;
                }
            }
        }
    }
}

void afficherDamier(SDL_Renderer *rendu, Damier *damier,RessourcesSDL *res) {
    SDL_Rect rect;
    rect.w = TAILLE_CASE;  // Largeur et hauteur de chaque case
    rect.h = TAILLE_CASE;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            rect.x = j * TAILLE_CASE;
            rect.y = i * TAILLE_CASE;

            // Dessiner la case (ou la texture de la case, si tu en as)
            if ((i + j) % 2 == 0) {
                SDL_SetRenderDrawColor(rendu, 255, 255, 255, 255);  // Blanc pour les cases claires
            } else {
                SDL_SetRenderDrawColor(rendu, 0, 0, 0, 255);  // Noir pour les cases sombres
            }
            SDL_RenderFillRect(rendu, &rect);

            // Dessiner le pion/dame
            switch (damier->cases[i][j].type) {
                case PION_BLANC:
                    SDL_RenderCopy(rendu, res->texturePionBlanc, NULL, &rect);
                    break;
                case PION_NOIR:
                    SDL_RenderCopy(rendu, res->texturePionNoir, NULL, &rect);
                    break;
                case DAME_BLANCHE:
                    SDL_RenderCopy(rendu, res->textureDameBlanc, NULL, &rect);
                    break;
                case DAME_NOIRE:
                    SDL_RenderCopy(rendu, res->textureDameNoir, NULL, &rect);
                    break;
                default:
                    break;
            }
        }
    }
}
void bouclePrincipale(RessourcesSDL *res) {
    Damier damier;
    initialiserDamier(&damier);

    SDL_Event event;
    bool continuer = true;

    while (continuer) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                continuer = false;
            }
        }

        SDL_RenderClear(res->rendu);
        afficherDamier(res->rendu, &damier,res);
        SDL_RenderPresent(res->rendu);
    }
}

bool initialiserJeu(JeuDeDames *jeu) {
if(!jeu) return false;
// Initialiser le plateau avec des cases vides
for (int i = 0; i < TAILLE; i++) {
for (int j = 0; j < TAILLE; j++) {
jeu->plateau[i][j] = VIDE;
}
}
// Placer les pions noirs (en haut)
for (int i = 0; i < 3; i++) {
for (int j = 0; j < TAILLE; j++) {
if ((i + j) % 2 == 1) { // Cases noires uniquement
jeu->plateau[i][j] = PION_NOIR;
}
}
}
// Placer les pions blancs (en bas)
for (int i = TAILLE - 3; i < TAILLE; i++) {
for (int j = 0; j < TAILLE; j++) {
if ((i + j) % 2 == 1) { // Cases noires uniquement
jeu->plateau[i][j] = PION_BLANC;
}
}
}
// Blanc commence
jeu->joueurActuel = 1;
jeu->partieTerminee = false;
return true;//indique que tout c'est bien passe
}


bool initialiserSDL(RessourcesSDL *res) {
    // Initialiser SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur lors de l'initialisation de SDL: %s\n", SDL_GetError());
        return false;
    }

    // Initialiser SDL_ttf
    if (TTF_Init() < 0) {
        printf("Erreur lors de l'initialisation de SDL_ttf: %s\n", TTF_GetError());
        SDL_Quit();
        return false;
    }

    // Créer la fenêtre
    res->fenetre = SDL_CreateWindow("Jeu de Dames",
                                    SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED,
                                    LARGEUR_FENETRE,
                                    HAUTEUR_FENETRE,
                                    SDL_WINDOW_SHOWN);
    if (!res->fenetre) {
        printf("Erreur de création de la fenêtre: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return false;
    }

    // Créer le renderer
    res->rendu = SDL_CreateRenderer(res->fenetre, -1, SDL_RENDERER_ACCELERATED);
    if (!res->rendu) {
        printf("Erreur de création du renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(res->fenetre);
        TTF_Quit();
        SDL_Quit();
        return false;
    }

    // Charger la police
    res->police = TTF_OpenFont("/home/dell/Bureau/le C/le dame/arial.ttf", 24);
    if (!res->police) {
        printf("Erreur de chargement de la police: %s\n", TTF_GetError());
        SDL_DestroyRenderer(res->rendu);
        SDL_DestroyWindow(res->fenetre);
        TTF_Quit();
        SDL_Quit();
        return false;
    }

    // Charger les textures
    SDL_Surface *surface = NULL;

    surface = SDL_LoadBMP("/home/dell/Bureau/le C/le dame/pion_blanc.bmp");
    if (!surface) {
        printf("Erreur de chargement de pion_blanc.bmp: %s\n", SDL_GetError());
        goto erreur;
    }
    res->texturePionBlanc = SDL_CreateTextureFromSurface(res->rendu, surface);
    SDL_FreeSurface(surface); // Libérer la surface après conversion
    if (!res->texturePionBlanc) {
        printf("Erreur de création de texture pion blanc: %s\n", SDL_GetError());
        goto erreur;
    }

    surface = SDL_LoadBMP("/home/dell/Bureau/le C/le dame/pion_noir.bmp");
    if (!surface) {
        printf("Erreur de chargement de pion_noir.bmp: %s\n", SDL_GetError());
        goto erreur;
    }
    res->texturePionNoir = SDL_CreateTextureFromSurface(res->rendu, surface);
    SDL_FreeSurface(surface);
    if (!res->texturePionNoir) {
        printf("Erreur de création de texture pion noir: %s\n", SDL_GetError());
        goto erreur;
    }

    surface = SDL_LoadBMP("/home/dell/Bureau/le C/le dame/dame_blanc.bmp");
    if (!surface) {
        printf("Erreur de chargement de dame_blanc.bmp: %s\n", SDL_GetError());
        goto erreur;
    }
    res->textureDameBlanc = SDL_CreateTextureFromSurface(res->rendu, surface);
    SDL_FreeSurface(surface);
    if (!res->textureDameBlanc) {
        printf("Erreur de création de texture dame blanc: %s\n", SDL_GetError());
        goto erreur;
    }

    surface = SDL_LoadBMP("/home/dell/Bureau/le C/le dame/dame_noir.bmp");
    if (!surface) {
        printf("Erreur de chargement de dame_noir.bmp: %s\n", SDL_GetError());
        goto erreur;
    }
    res->textureDameNoir = SDL_CreateTextureFromSurface(res->rendu, surface);
    SDL_FreeSurface(surface);
    if (!res->textureDameNoir) {
        printf("Erreur de création de texture dame noir: %s\n", SDL_GetError());
        goto erreur;
    }

    return true; // Tout s'est bien passé

erreur:
    // Nettoyage en cas d'erreur
    if (res->texturePionBlanc) SDL_DestroyTexture(res->texturePionBlanc);
    if (res->texturePionNoir) SDL_DestroyTexture(res->texturePionNoir);
    if (res->textureDameBlanc) SDL_DestroyTexture(res->textureDameBlanc);
    if (res->textureDameNoir) SDL_DestroyTexture(res->textureDameNoir);
    if (res->police) TTF_CloseFont(res->police);
    SDL_DestroyRenderer(res->rendu);
    SDL_DestroyWindow(res->fenetre);
    TTF_Quit();
    SDL_Quit();
    return false;
}

void afficherJoueurActuel(RessourcesSDL *res, JeuDeDames *jeu) {
    if (res->police) {
        SDL_Color couleur = {255, 255, 255, 255}; // Blanc
        char texte[50];
        sprintf(texte, "Joueur: %s", (jeu->joueurActuel == 1) ? "Blanc" : "Noir");
        SDL_Surface *surface = TTF_RenderText_Solid(res->police, texte, couleur);
        if (surface) {
            SDL_Texture *texture = SDL_CreateTextureFromSurface(res->rendu, surface);
            if (texture) {
                SDL_Rect rect = {10, 10, surface->w, surface->h};
                SDL_RenderCopy(res->rendu, texture, NULL, &rect);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(surface);
        }
    }
}
void dessinerPlateau(RessourcesSDL *res, JeuDeDames *jeu) {
// Effacer l'écran avec une couleur de fond
SDL_SetRenderDrawColor(res->rendu, 150, 100, 50, 255); // Couleur bois clair
SDL_RenderClear(res->rendu);
// Dessiner les cases du damier (alternance de cases claires et sombres)
for (int i = 0; i < TAILLE; i++) {
for (int j = 0; j < TAILLE; j++) {
SDL_Rect rect = {j * TAILLE_CASE, i * TAILLE_CASE, TAILLE_CASE,TAILLE_CASE};
if ((i + j) % 2 == 0) {
// Case blanche
SDL_SetRenderDrawColor(res->rendu, 240, 217, 181, 255); // Beige clair
} else {
// Case noire
SDL_SetRenderDrawColor(res->rendu, 100, 65, 23, 255); // Marron foncé
}
SDL_RenderFillRect(res->rendu, &rect);
// Mettre en évidence la case sélectionnée
if (jeu->pieceSel.ligne == i && jeu->pieceSel.colonne == j) {
SDL_SetRenderDrawColor(res->rendu, 0, 255, 0, 100); // Vert transparent
SDL_RenderFillRect(res->rendu, &rect);
}
// Dessiner les pièces
if (jeu->plateau[i][j] != VIDE) {
SDL_Texture *texture = NULL;
switch (jeu->plateau[i][j]) {
case PION_BLANC:
texture = res->texturePionBlanc;
break;
case PION_NOIR:
texture = res->texturePionNoir;
break;
case DAME_BLANCHE:
texture = res->textureDameBlanc;
break;
case DAME_NOIRE:
texture = res->textureDameNoir;
break;
}
if (texture) {
SDL_Rect destRect = {
j * TAILLE_CASE + 10,
i * TAILLE_CASE + 10,
TAILLE_CASE - 20,
TAILLE_CASE - 20
};
SDL_RenderCopy(res->rendu, texture, NULL, &destRect);
}
}
}
}
// Afficher le joueur actuel
/*if (res->police) {
SDL_Color couleur = {255, 255, 255, 255};
char texte[50];
sprintf(texte, "Joueur: %s", (jeu->joueurActuel == 1) ? "Blanc" : "Noir");
SDL_Surface *surface = TTF_RenderText_Solid(res->police, texte, couleur);
if (surface) {
SDL_Texture *texture = SDL_CreateTextureFromSurface(res->rendu, surface);
if (texture) {
SDL_Rect rect = {10, 10, surface->w, surface->h};
SDL_RenderCopy(res->rendu, texture, NULL, &rect);
SDL_DestroyTexture(texture);
}
SDL_FreeSurface(surface);
}
}*/
afficherJoueurActuel(res, jeu);

// Afficher le rendu à l'écran
SDL_RenderPresent(res->rendu);
}


bool estPositionValide(Position pos) {
    return (pos.ligne >= 0 && pos.ligne < TAILLE && pos.colonne >= 0 && pos.colonne < TAILLE);
}



bool estMouvementValide(JeuDeDames *jeu, Mouvement mouv) {
// Vérifier que les positions sont valides
if (!estPositionValide(mouv.debut) || !estPositionValide(mouv.fin)) {
return false;
}
// Vérifier que la case de départ contient un pion du joueur actuel
int pieceDepart = jeu->plateau[mouv.debut.ligne][mouv.debut.colonne];
if (jeu->joueurActuel == 1 && (pieceDepart != PION_BLANC && pieceDepart !=
DAME_BLANCHE)) {
return false;
}
if (jeu->joueurActuel == 2 && (pieceDepart != PION_NOIR && pieceDepart !=
DAME_NOIRE)) {
return false;
}
// Vérifier que la case d'arrivée est vide
if (jeu->plateau[mouv.fin.ligne][mouv.fin.colonne] != VIDE) {
return false;
}
// Vérifier que la case d'arrivée est une case noire
if ((mouv.fin.ligne + mouv.fin.colonne) % 2 == 0) {
return false;
}
// Calcul des différences entre positions
int diffLigne = mouv.fin.ligne - mouv.debut.ligne;
int diffColonne = mouv.fin.colonne - mouv.debut.colonne;
// Vérification du sens du mouvement pour les pions
if (pieceDepart == PION_BLANC && diffLigne >= 0) {
return false; // Les pions blancs ne peuvent aller que vers le haut
}
if (pieceDepart == PION_NOIR && diffLigne <= 0) {
return false; // Les pions noirs ne peuvent aller que vers le bas
}
// Mouvement simple en diagonale
if (abs(diffLigne) == 1 && abs(diffColonne) == 1) {
return true;
}
// Mouvement de prise (saut par-dessus un pion adverse)
if (abs(diffLigne) == 2 && abs(diffColonne) == 2) {
// Position du pion à prendre
Position positionPrise;
positionPrise.ligne = mouv.debut.ligne + diffLigne / 2;
positionPrise.colonne = mouv.debut.colonne + diffColonne / 2;
// Vérifier qu'il y a un pion adverse à prendre
int piecePrise = jeu->plateau[positionPrise.ligne][positionPrise.colonne];
if (jeu->joueurActuel == 1 && (piecePrise == PION_NOIR || piecePrise ==
DAME_NOIRE)) {
return true;
}
if (jeu->joueurActuel == 2 && (piecePrise == PION_BLANC || piecePrise ==
DAME_BLANCHE)) {
return true;
}
}
// Mouvement invalide dans tous les autres cas
return false;
}



void effectuerMouvement(JeuDeDames *jeu, Mouvement mouv) {
// Déplacer le pion
jeu->plateau[mouv.fin.ligne][mouv.fin.colonne] = jeu->plateau[mouv.debut.ligne][mouv.debut.colonne];
jeu->plateau[mouv.debut.ligne][mouv.debut.colonne] = VIDE;
// Vérifier si c'est une prise
if (abs(mouv.fin.ligne - mouv.debut.ligne) == 2) {
// Calculer la position du pion pris
int ligneIntermediaire = (mouv.debut.ligne + mouv.fin.ligne) / 2;
int colonneIntermediaire = (mouv.debut.colonne + mouv.fin.colonne) / 2;
// Enlever le pion pris
jeu->plateau[ligneIntermediaire][colonneIntermediaire] = VIDE;
}
// Vérifier si un pion doit être transformé en dame
if (jeu->plateau[mouv.fin.ligne][mouv.fin.colonne] == PION_BLANC && mouv.fin.ligne
== 0) {
jeu->plateau[mouv.fin.ligne][mouv.fin.colonne] = DAME_BLANCHE;
}
if (jeu->plateau[mouv.fin.ligne][mouv.fin.colonne] == PION_NOIR && mouv.fin.ligne
== TAILLE - 1) {
jeu->plateau[mouv.fin.ligne][mouv.fin.colonne] = DAME_NOIRE;
}
// Changer de joueur
jeu->joueurActuel = (jeu->joueurActuel == 1) ? 2 : 1;
}


bool verifierFinPartie(JeuDeDames *jeu) {
int compteurBlancs = 0, compteurNoirs = 0;
// Compter les pions restants
for (int i = 0; i < TAILLE; i++) {
for (int j = 0; j < TAILLE; j++) {
if (jeu->plateau[i][j] == PION_BLANC || jeu->plateau[i][j] == DAME_BLANCHE) {
compteurBlancs++;
} else if (jeu->plateau[i][j] == PION_NOIR || jeu->plateau[i][j] == DAME_NOIRE) {
compteurNoirs++;
}
}
}
// Si un joueur n'a plus de pions, la partie est terminée
if (compteurBlancs == 0) {
printf("Les noirs ont gagné!\n");
return true;
} else if (compteurNoirs == 0) {
printf("Les blancs ont gagné!\n");
return true;
}
return false;
}


void libererSDL(RessourcesSDL *res) {
    /*SDL_DestroyTexture(res->texturePionBlanc);
    SDL_DestroyTexture(res->texturePionNoir);
    SDL_DestroyTexture(res->textureDameBlanc);
    SDL_DestroyTexture(res->textureDameNoir);
    TTF_CloseFont(res->police);
    SDL_DestroyRenderer(res->rendu);
    SDL_DestroyWindow(res->fenetre);
    TTF_Quit();
    SDL_Quit();*/
    
    if (res->texturePionBlanc) SDL_DestroyTexture(res->texturePionBlanc);
    if (res->texturePionNoir) SDL_DestroyTexture(res->texturePionNoir);
    if (res->textureDameBlanc) SDL_DestroyTexture(res->textureDameBlanc);
    if (res->textureDameNoir) SDL_DestroyTexture(res->textureDameNoir);
    if (res->police) TTF_CloseFont(res->police);
    if (res->rendu) SDL_DestroyRenderer(res->rendu);
    if (res->fenetre) SDL_DestroyWindow(res->fenetre);
    TTF_Quit();
    SDL_Quit();
}

Position obtenirPositionClic(int x, int y) {
    Position pos;
    pos.ligne = y / TAILLE_CASE;
    pos.colonne = x / TAILLE_CASE;
    return pos;
}

void selectionnerPiece(JeuDeDames *jeu, Position pos) {
    /*if (jeu->plateau[pos.ligne][pos.colonne] != VIDE) {
        jeu->pieceSel = pos;  // Sélectionner la pièce
    }*/
    if (jeu->plateau[pos.ligne][pos.colonne] != VIDE) {
        int piece = jeu->plateau[pos.ligne][pos.colonne];
        // Vérifie que la pièce appartient au joueur actuel
        if ((jeu->joueurActuel == 1 && (piece == PION_BLANC || piece == DAME_BLANCHE)) ||
            (jeu->joueurActuel == 2 && (piece == PION_NOIR || piece == DAME_NOIRE))) {
            jeu->pieceSel = pos;  // Sélectionner la pièce
        }
    }
}

                     /*Position obtenirPositionClic(int x, int y) {
                     Position pos;
                     pos.ligne = y / TAILLE_CASE;
    			pos.colonne = x / TAILLE_CASE;
    			return pos;
			}

			void selectionnerPiece(JeuDeDames *jeu, Position pos) {
    			// Vérifier que la case contient une pièce du joueur actuel
    			if ((jeu->joueurActuel == 1 && (jeu->plateau[pos.ligne][pos.colonne] == PION_BLANC || 										jeu->plateau[pos.ligne][pos.colonne] == DAME_BLANCHE)) ||
        			(jeu->joueurActuel == 2 && (jeu->plateau[pos.ligne][pos.colonne] == PION_NOIR || jeu-			>plateau[pos.ligne][pos.colonne] == DAME_NOIRE))) {
        			jeu->pieceSel = pos; // Sélectionner la pièce
   			 }
			}

			void deplacerPiece(JeuDeDames *jeu, Position pos) {
    			Mouvement mouv = {jeu->pieceSel, pos};
    			if (estMouvementValide(jeu, mouv)) {
     			   effectuerMouvement(jeu, mouv);
       			 jeu->pieceSel.ligne = -1; // Désélectionner la pièce
        			jeu->pieceSel.colonne = -1;
    			}
			}

			void libererSDL(RessourcesSDL *res) {
    			SDL_DestroyTexture(res->texturePionBlanc);
    			SDL_DestroyTexture(res->texturePionNoir);
    			SDL_DestroyTexture(res->textureDameBlanc);
    			SDL_DestroyTexture(res->textureDameNoir);
    			TTF_CloseFont(res->police);
    			SDL_DestroyRenderer(res->rendu);
    			SDL_DestroyWindow(res->fenetre);
   			 TTF_Quit();
   			 SDL_Quit();
				}*/
void deplacerPiece(JeuDeDames *jeu, Position pos) {
    Mouvement mouv = {jeu->pieceSel, pos};
    if (estMouvementValide(jeu, mouv)) {
        effectuerMouvement(jeu, mouv);
        jeu->pieceSel.ligne = -1; // Désélectionner la pièce
        jeu->pieceSel.colonne = -1;
    }
    }
void afficherMessageFin(RessourcesSDL *ressources, JeuDeDames *jeu){}


int main(int argc, char *argv[]) {
    JeuDeDames jeu;
    RessourcesSDL ressources;
    bool continuer = true;
    SDL_Event event;

    // Initialiser le jeu
    if (!initialiserJeu(&jeu)) {
        printf("Erreur lors de l'initialisation du jeu !\n");
        return EXIT_FAILURE;
    }

    // Initialiser SDL
    if (!initialiserSDL(&ressources)) {
        printf("Erreur lors de l'initialisation de SDL !\n");
        return EXIT_FAILURE;
    }

    // Initialiser la sélection de pièce
    jeu.pieceSel.ligne = -1;
    jeu.pieceSel.colonne = -1;

    // Boucle principale du jeu
    while (continuer && !jeu.partieTerminee) {
        // Dessiner le plateau
        dessinerPlateau(&ressources, &jeu);

        // Gérer les événements
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    continuer = false;
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        Position pos = obtenirPositionClic(event.button.x, event.button.y);
                        
                        if (jeu.pieceSel.ligne == -1 || jeu.pieceSel.colonne == -1) {
                            selectionnerPiece(&jeu, pos);
                        } else {
                            deplacerPiece(&jeu, pos);
                        }
                    }
                    break;

                default:
                    break;
            }
        }

        // Limiter la fréquence d'images (~60 FPS)
        SDL_Delay(16);
    }

    // Affichage de la fin de partie
    if (jeu.partieTerminee) {
        afficherMessageFin(&ressources, &jeu); // Fonction hypothétique pour afficher un message
        SDL_Delay(3000); // Pause pour laisser le joueur voir le message
    }

    // Libérer les ressources SDL proprement
    libererSDL(&ressources);
    return EXIT_SUCCESS;
}




