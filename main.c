#include <stdio.h>

#define MAX 30 // Maksimum aday sayisi

// preferences[i][j] : i adayini j adayina tercih eden kisi sayisi
int preferences[MAX][MAX];

// locked[i][j] : 1 ise yonlu graph'ta i adayindan j adayina dogru ok vardir
int locked[MAX][MAX];

int pointed[MAX]; // kendilerine isaret eden en az bir ok bulunan adaylar

typedef struct pair {
    int winner;
    int loser;
} pair;

pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

void f_mtrx_print(int mtrx[][MAX], int nrows, int ncolumns);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
int is_cycle(int start, int end);

int main()
{
    int voter_count;

    printf("Number of candidates: ");
    scanf("%d", &candidate_count);
    if(candidate_count > MAX) {
        printf("Maximum number of candidates is %d\n", MAX);
        return 2;
    }

    pair_count = 0;
    printf("Number of voters: ");
    scanf("%d", &voter_count);

    // secmenlerin oylari aliniyor
    for(int i = 0; i < voter_count; i++) {
        // ranks[i] : secmenin i. tercihi (ranks[0] ilk tercih, ranks[1] 2. tercih vb.)
        int ranks[candidate_count];

        printf("Ranks: ");
        for(int j = 0; j < candidate_count; j++) {
            int choice;

            scanf("%d", &choice);
            if(choice < 0 || choice >= candidate_count) {
                printf("Invalid vote.\n");
                return 3;
            }
            ranks[j] = choice;
        }
        record_preferences(ranks);
    }
    printf("\npreferences[][]:\n");
    f_mtrx_print(preferences, candidate_count, candidate_count);
    add_pairs();
    sort_pairs();
    lock_pairs();
    printf("\nlocked[][]:\n");
    f_mtrx_print(locked, candidate_count, candidate_count);
    printf("\npointed[][]:\n");
    for(int i = 0; i < candidate_count; i++)
        printf("%3d ", i);
    printf("\n");
    for(int i = 0; i < candidate_count; i++)
        printf(" %c%c ", 196, 196);
    printf("\n");
    for(int i = 0; i < candidate_count; i++)
        printf("%3d ", pointed[i]);

    printf("\n\nWinner: ");
    print_winner();

    return 0;
}

// Formatli bicimde matris yazdirma
void f_mtrx_print(int mtrx[][MAX], int nrows, int ncolumns)
{
    printf("     ");
    for(int i = 0; i < ncolumns; i++) {
        printf("%3d ", i);
    }
    printf("\n     ");
    for(int i = 0; i < ncolumns; i++) {
        printf(" %c%c ", 196, 196);
    }
    printf("\n");
    for(int i = 0; i < nrows; i++) {
        printf("%3d| ", i);
        for(int j = 0; j < ncolumns; j++) {
            printf("%3d ", mtrx[i][j]);
        }
        printf("\n");
    }
}

// Bir secmenin oylari verildiginde, ranks[] dizisine gore preferences matrisini gunceller
void record_preferences(int ranks[])
{
    for(int i = candidate_count - 2; i >= 0; i--) {
        for(int j = i + 1; j < candidate_count; j++) {
            preferences[ranks[i]][ranks[j]]++;
        }
    }
    return;
}

// Biri digerine tercih edilen adaylardan pair olusturur
void add_pairs(void)
{
    for(int i = 0; i < candidate_count - 1; i++) {
        for(int j = i + 1; j < candidate_count; j++) {
            int winner, loser;
            // i ve j arasinda preferences matrisine bakarak winner-loser belirle, esitlik durumunda pair olusturma
            if(preferences[i][j] > preferences[j][i]) {
                winner = i;
                loser = j;
            } else if(preferences[i][j] < preferences[j][i]) {
                winner = j;
                loser = i;
            } else {
                continue; // esitlik durumunda atla
            }
            // olusturulacak pair'in winner-loser atamalarini yap, pair sayisini artir
            pairs[pair_count].winner = winner;
            pairs[pair_count++].loser = loser;
        }
    }
    return;
}

// pair'lari en yuksek farkla kazanandan en az farkla kazanana dogru sirala
void sort_pairs(void)
{
    for(int i = 0; i < pair_count - 1; i++) {
        int max = i;
        for(int j = i + 1; j < pair_count; j++) {
            // ne kadar farkla kazandigina preferences matrisinden bakiyoruz
            int j_score = preferences[pairs[j].winner][pairs[j].loser];
            int max_score = preferences[pairs[max].winner][pairs[max].loser];

            if(j_score > max_score) {
                max = j;
            }
        }
        pair tmp = pairs[i];
        pairs[i] = pairs[max];
        pairs[max] = tmp;
    }
    return;
}

// pair'in kaybedeninden kazananina dogru bir yol var mi kontrol eder, eger varsa 1 dondurur
// boyle bir yol graph'ta dongu olusmasina sebep olur
int is_cycle(int start, int end)
{
    if(start == end) { // bir yol bulundu
        return 1;
    }

    for(int i = 0; i < candidate_count; i++) {
        if(locked[start][i]) {
            return is_cycle(i, end); // ayni fonksiyonu su anki node'a bagli tum node'lar icin cagir
        }
    }

    return 0; // dongu olusmuyor
}

// pair'lari sirayla donguye sebep olmayacak sekilde kilitler
void lock_pairs(void)
{
    if(pair_count > 0) {
        locked[pairs[0].winner][pairs[0].loser] = 1;
        pointed[pairs[0].loser] = 1;
    }

    for(int i = 1; i < pair_count; i++) {
        int winner = pairs[i].winner;
        int loser = pairs[i].loser;

        // pair'i kilitlemek donguye sebep olacak mi kontrol et
        if(!is_cycle(loser, winner)) {
            locked[winner][loser] = 1;
            pointed[loser] = 1;
        }
    }

    return;
}

// Secimin kazananini ekrana yazar
void print_winner(void)
{
    for(int i = 0; i < candidate_count; i++) {
        if(!pointed[i]) // winner is the candidate with no arrow pointing towards
        {
            printf("%d\n", i);
        }
    }

    return;
}
