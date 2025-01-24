#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h> // For the sleep function

// ANSI kaçış kodları
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"

// Taşları temsil eden karakterler
char pieces[] = {'A', 'B', 'C', 'D', 'E'};

// Yenilen taşlara göre puan hesaplayan fonksiyon
int puan_hesapla(int yenilentas[]) {
    int puan = yenilentas[0];
    for (int i = 0; i < 5; i++) {
        if ( puan > yenilentas[i] ){
            puan = yenilentas[i];
        } // Taşların indexine göre puan hesaplama
    }
    return puan;
}

// Harfe göre renk seçme fonksiyonu
const char* get_color(char c) {
    switch(c) {
        case 'A': return RED;
        case 'B': return GREEN;
        case 'C': return YELLOW;
        case 'D': return BLUE;
        case 'E': return MAGENTA;
        default: return RESET;
    }
}

// İki oyuncu için yenilen taşların tutulduğu diziler
int yenilentas1[5] = {0};
int yenilentas2[5] = {0};
int undoyenilentas1[5] = {0};
int redoyenilentas1[5] = {0};
int undoyenilentas2[5] = {0};
int redoyenilentas2[5] = {0};
// verilen yerin harflerlerden biri mi oldugunu kontrol eden bool 
bool isThisPiece(char c){
    int i;
    if ( c >= 'A' && c <= 'E'){
        return true ;
    }
    else{
        return false ;
    }
}

// Yenilen taşları tutan fonksiyon
void tasyeme(char piece, int player) {
    int *yenilentas = (player == 1) ? yenilentas1 : yenilentas2;
    for (int i = 0; i < 5; i++) {
        if (pieces[i] == piece) {
            yenilentas[i]++;
            break;
        }
    }
}

//puan heasplama 
void determine_winner() {
    int puan1 = puan_hesapla(yenilentas1);
    int puan2 = puan_hesapla(yenilentas2);

    if (puan1 > puan2) {
        printf("Oyuncu 1 kazandı!\n\n");
    } else if (puan2 > puan1) {
        printf("Oyuncu 2 kazandı!\n\n");
    } else {
        // Puanlar eşitse taş sayısına bak
        int tas1_count = 0, tas2_count = 0;
        for (int i = 0; i < 5; i++) {
            tas1_count += yenilentas1[i];
            tas2_count += yenilentas2[i];
        }
        if (tas1_count > tas2_count) {
            printf("Oyuncu 1 kazandı (taş sayısı ile)!\n\n");
        } else if (tas2_count > tas1_count) {
            printf("Oyuncu 2 kazandı (taş sayısı ile)!\n\n");
        } else {
            printf("Oyun berabere!\n\n");
        }
    }
}

//matris kopyalama islemi 
void copy_matrix(char **src, char **dest , int n )  {
    for (int i = 0; i < n; i++) {
        memcpy(dest[i], src[i], n * sizeof(char));
    }

}

//undodizilerine yenilen taslari atan fonksiyon
void copy_yenilentastoundo(){
    for (int i = 0; i < 5; i++) {
        undoyenilentas1[i] = yenilentas1[i];
        undoyenilentas2[i] = yenilentas2[i];
    }
}

//redo dizilerine yenilen taslari atan fonksiyon
void copy_yenilentastoredo(){
    for (int i = 0; i < 5; i++) {
        redoyenilentas1[i] = yenilentas1[i];
        redoyenilentas2[i] = yenilentas2[i];
    }
}

//undo dan yenilentasa
void copy_undotoyenilen(){
    for (int i = 0; i < 5; i++) {
        yenilentas1[i] = undoyenilentas1[i];
        yenilentas2[i] = undoyenilentas2[i];
    }
}

//redo dan yenilentasa
void copy_redotoyenilen(){
    for (int i = 0; i < 5; i++) {
       yenilentas1[i] = redoyenilentas1[i];
       yenilentas2[i] = redoyenilentas2[i];
    }
}

//Oyunu kaydetme
void save_game(char **board, int n) {
    FILE *file = fopen("game_save.txt", "w");
    if (file == NULL) {
        printf("Dosya açılamadı!\n");
        return;
    }

    // Tahta boyutunu kaydet
    fprintf(file, "%d\n", n);

    // Tahtayı kaydet
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            char cell = board[i][j];
            if (cell == ' ') {
                cell = 'R'; // Boşlukları 'R' ile değiştir
            }
            fprintf(file, "%c ", cell);
        }
        fprintf(file, "\n");
    }

    // Yenilen taşları kaydet
    for (int i = 0; i < 5; i++) {
        fprintf(file, "%d ", yenilentas1[i]);
    }
    fprintf(file, "\n");

    for (int i = 0; i < 5; i++) {
        fprintf(file, "%d ", yenilentas2[i]);
    }
    fprintf(file, "\n");

    fclose(file);
    printf("Oyun başarıyla kaydedildi!\n");
}

//Kaydedilen oyunu yukleme 
void load_game(char ***board, int *n) {
    FILE *file = fopen("game_save.txt", "r");
    if (file == NULL) {
        printf("Dosya açılamadı!\n");
        return;
    }

    // Tahta boyutunu oku
    fscanf(file, "%d", n);

    // Tahtayı bellekten ayır
    *board = malloc((*n) * sizeof(char *));
    for (int i = 0; i < *n; i++) {
        (*board)[i] = malloc((*n) * sizeof(char));
    }

    // Tahtayı oku
    for (int i = 0; i < *n; i++) {
        for (int j = 0; j < *n; j++) {
            char cell;
            fscanf(file, " %c", &cell);
            if (cell == 'R') {
                cell = ' '; // 'R' karakterini boşluk ile değiştir
            }
            (*board)[i][j] = cell;
        }
    }

    // Yenilen taşları oku
    for (int i = 0; i < 5; i++) {
        fscanf(file, "%d", &yenilentas1[i]);
    }

    for (int i = 0; i < 5; i++) {
        fscanf(file, "%d", &yenilentas2[i]);
    }

    fclose(file);
    printf("Oyun başarıyla yüklendi!\n");
}

// Her oyuncunun yediği taşları ve miktarlarını ekrana yazdıran fonksiyon
void print_yenilen_taslar() {
    printf("Oyuncu 1'in Yediği Taşlar ve Miktarları:\n");
    for (int i = 0; i < 5; i++) {
        printf("%s%c: %d%s\n", get_color(pieces[i]), pieces[i], yenilentas1[i], RESET);
    }
    printf("\n");

    printf("Oyuncu 2'nin Yediği Taşlar ve Miktarları:\n");
    for (int i = 0; i < 5; i++) {
        printf("%s%c: %d%s\n", get_color(pieces[i]), pieces[i], yenilentas2[i], RESET);
    }
    printf("\n");
}

//Taslari  yerlestirme
void shuffle(char *array, int n) {
    if (n > 1) {
        for (int i = 0; i < n - 1; i++) {
            int j = i + rand() / (RAND_MAX / (n - i) + 1);
            char t = array[i];
            array[i] = array[j];
            array[j] = t;
        }
    }
}

//Tas yerlestirme
void place_pieces(char **board, int n) {
    int i, j;
    int center_start = n / 2 - 1;
    int num_pieces = n * n - 4;  // 4 hücre boş kalacak

    // Taşları temsil eden karakterler
    char pieces[] = {'A', 'B', 'C', 'D', 'E'};
    int num_types = sizeof(pieces) / sizeof(pieces[0]);
    
    // Her taş türünün sayısı
    int num_each_type = num_pieces / num_types;
    int remainder = num_pieces % num_types;

    // Tüm taşları içeren bir dizi oluştur
    char *all_pieces = (char *)malloc(num_pieces * sizeof(char));
    int index = 0;
    for (i = 0; i < num_types; i++) {
        for (j = 0; j < num_each_type; j++) {
            all_pieces[index++] = pieces[i];
        }
    }
    // Artan taşları ekle
    for (i = 0; i < remainder; i++) {
        all_pieces[index++] = pieces[i];
    }

    // Taşları karıştır
    shuffle(all_pieces, num_pieces);

    // Tüm hücreleri boş karakterle doldur
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            board[i][j] = ' ';
        }
    }

    // Taşları sırayla yerleştir
    index = 0;
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            // Orta 4 hücreyi atla
            if ((i == center_start || i == center_start + 1) &&
                (j == center_start || j == center_start + 1)) {
                continue;
            }
            board[i][j] = all_pieces[index++];
        }
    }

    // Taşları içeren diziyi serbest bırak
    free(all_pieces);
}

// Oyun alanını ekrana basan fonksiyon
void print_board(char **board, int n) {
    int i, j;

    system("clear");
    // Sütun numaralarını yazdırma
    printf("     ");
    for (j = 0; j < n; j++) {
        printf("%2d  ", j+1);
    }
    printf("\n");

    // Üst çizgi
    printf("   ");
    for (j = 0; j < n; j++) {
        printf("----");
    }
    printf("-\n");

    // Matrisin hücre sınırlarını ve taşları basma
    for (i = 0; i < n; i++) {
        printf("%2d |", i+1);  // Satır numaraları
        for (j = 0; j < n; j++) {
            printf(" %s%c%s |", get_color(board[i][j]), board[i][j], RESET);  // Hücre içeriği ve sağa sınır
        }
        printf("\n");  // Bir satır tamamlandığında alt satıra geç
        printf("   ");
        for (j = 0; j < n; j++) {
            printf("----");  // Hücre alt sınırı
        }
        printf("-\n");  // Satır sonunda ekstra sınır çizgisi
    }
    
    // Oyuncuların puanlarını yazdırma
    int puan1 = puan_hesapla(yenilentas1);
    int puan2 = puan_hesapla(yenilentas2);
    printf("Oyuncu 1 Puanı: %d\n", puan1);
    printf("Oyuncu 2 Puanı: %d\n", puan2);
    print_yenilen_taslar();
}

// Oyuncu hamlesi fonksiyonu
void player_move(char **board,char **undoboard ,char **redoboard ,int n , int player , int* pcikis1 ) {
    int row, col , move_row, move_col , undoislem , flag =0  ;
    //undo islemi icin matrisi kopyalama 
    copy_matrix(board , undoboard , n );
    copy_yenilentastoundo();
    // Oyuncudan hamle koordinatlarını alma
    printf("Hamle yapmak istediğiniz hücrenin satır ve sütun numarasını girin: ");
    scanf("%d %d", &row, &col);

    // Hücrenin geçerli olup olmadığını kontrol etme
    if (row <= 0 || row > n || col <= 0 || col > n) {
        printf("Geçersiz hucre! Lütfen geçerli bir hücre seçin.\n");
        scanf("%d %d", &row, &col);
    }

    // Hücre boş mu diye kontrol etme
    if (board[row-1][col-1] == ' ') {
        printf("Seçtiğiniz hücre bos ! Lütfen dolu  bir hücre seçin.\n");
        scanf("%d %d", &row, &col);
    }
    
    printf("Hamle yapacaginiz yerin satir ve sutun numarasini giriniz1: ");
    scanf("%d %d", &move_row, &move_col);

    // Hücrenin geçerli olup olmadığını kontrol etme
    if (move_row <= 0 || move_row > n || move_col <= 0 || move_col > n) {
        printf("Geçersiz hucre! Lütfen geçerli bir hücre seçin.\n");
        scanf("%d %d", &move_row, &move_col);
    }
    

    if (board[move_row-1][move_col-1] == ' ') {
        board[move_row-1][move_col-1] = board[row-1][col-1] ;
        board[row-1][col-1] = ' ';
        if ( row == move_row ){
            //burada bir tane dizinin icine atip sonrasinda oradakileri sayabilirim 
            if(move_col- col == 2){
                tasyeme( board[row-1][col] , player);
                board[row-1][col] = ' ';
                }
            else if( move_col - col == -2 ){
                tasyeme( board[row-1][col-2] , player);
                board[row-1][col-2] = ' ';
                }
        }
        else if ( col == move_col ){
            if(move_row - row == 2){
                tasyeme( board[row][col-1] , player );
                board[row][col-1] = ' ';
                }
            else if(move_row - row == -2 ){
                tasyeme( board[row-2][col-1] , player );
                board[row-2][col-1] = ' ';
                }
        }

    }
    print_board(board, n);
    if(flag == 0){
        printf("undo:1\ndevam :2\noyundan cik:5\n");
      scanf("%d",&undoislem);
        if(undoislem == 1  ){
                flag =1 ;
                copy_yenilentastoredo();
                copy_undotoyenilen();
                copy_matrix(board , redoboard,n);
                copy_matrix(undoboard , board,n);
                print_board(board, n);
                printf("Redo:3\nYeni hamle:4\n ");
                scanf("%d",&undoislem);
                if(undoislem == 3){
                    copy_yenilentastoundo();
                    copy_redotoyenilen();
                    copy_matrix(board , undoboard, n );
                    copy_matrix(redoboard , board, n );
                    print_board(board,n);
                }
                else if(undoislem == 4){
                    copy_matrix(board , undoboard , n );
                    copy_yenilentastoundo();
                    printf("Hamle yapmak istediğiniz hücrenin satır ve sütun numarasını girin: ");
                    scanf("%d %d", &row, &col);
                    printf("Hamle yapacaginiz yerin satir ve sutun numarasini giriniz1: ");
                    scanf("%d %d", &move_row, &move_col);
                    if (board[move_row-1][move_col-1] == ' ') {
                        board[move_row-1][move_col-1] = board[row-1][col-1] ;
                        board[row-1][col-1] = ' ';
                    if ( row == move_row ){
                        //burada bir tane dizinin icine atip sonrasinda oradakileri sayabilirim 
                        if(move_col- col == 2){
                            tasyeme( board[row-1][col] , player);
                            board[row-1][col] = ' ';
                        }
                        else if( move_col - col == -2 ){
                        tasyeme( board[row-1][col-2] , player);
                        board[row-1][col-2] = ' ';
                        }
                    }
                    else if ( col == move_col ){
                        if(move_row - row == 2){
                        tasyeme( board[row][col-1] , player );
                        board[row][col-1] = ' ';
                        }
                        else if(move_row - row == -2 ){
                        tasyeme( board[row-2][col-1] , player );
                        board[row-2][col-1] = ' ';
                        }
                    }
                    }
                    print_board(board,n);
                }
        }
    
    }
                
            
        if(undoislem == 5 ){
            printf("\n\nOYUN SONA ERDI !!\n\n");
            // Oyuncuların puanlarını yazdırma
            int puan1 = puan_hesapla(yenilentas1);
            int puan2 = puan_hesapla(yenilentas2);
            printf("Oyuncu 1 Puanı: %d\n", puan1);
            printf("Oyuncu 2 Puanı: %d\n", puan2);
            print_yenilen_taslar();
            determine_winner();
            save_game(board,n);
            *pcikis1 = 1;
        }
        
    
    while((move_row-3 >= 0) &&(board[move_row-3][move_col-1] == ' ' && board[move_row-2][move_col-1] != ' ' ) || ((move_col-3 >= 0 ) && board[move_row-1][move_col-3] == ' ' && board[move_row-1][move_col-2] != ' ')|| ((move_row + 1 < n) && board[move_row+1][move_col-1] == ' ' && board[move_row][move_col-1] != ' '  )|| ((move_col+1 < n) && board[move_row-1][move_col] != ' '  && board[move_row-1][move_col+1] == ' ')){
        
        copy_matrix(board , undoboard , n );
        copy_yenilentastoundo();
        row = move_row;
        col = move_col; 

        printf("Hamle yapacaginiz yerin satir ve sutun numarasini giriniz2 : ");
        scanf("%d %d", &move_row, &move_col);
        // Hücrenin geçerli olup olmadığını kontrol etme
        if (move_row <= 0 || move_row > n || move_col <= 0 || move_col > n) {
            printf("Geçersiz hucre! Lütfen geçerli bir hücre seçin.\n");
            scanf("%d %d", &move_row, &move_col);
        }

        if (board[move_row-1][move_col-1] == ' ') {
            board[move_row-1][move_col-1] = board[row-1][col-1] ;
            board[row-1][col-1] = ' ';
        if ( row == move_row ){
          //burada bir tane dizinin icine atip sonrasinda oradakileri sayabilirim 
            if(move_col- col == 2){
                tasyeme( board[row-1][col] , player);
                board[row-1][col] = ' ';
                }
            else if( move_col - col == -2 ){
                tasyeme( board[row-1][col-2] , player);
                board[row-1][col-2] = ' ';
                }
        }
        else if ( col == move_col ){
            if(move_row - row == 2){
                tasyeme( board[row][col-1] , player );
                board[row][col-1] = ' ';
                }
            else if(move_row - row == -2 ){
                tasyeme( board[row-2][col-1] , player );
                board[row-2][col-1] = ' ';
                }
        }
        }
        print_board(board, n);
        printf("undo:1\ndevam :2\noyundan cik:5\n");
        scanf("%d",&undoislem);
        if(flag == 0){
            if(undoislem == 1 ){
                flag = 1;
                copy_yenilentastoredo();
                copy_undotoyenilen();
                copy_matrix(board , redoboard,n);
                copy_matrix(undoboard , board,n);
                print_board(board, n);
                printf("Redo:3\nYeni hamle:4\n ");
                scanf("%d",&undoislem);
                if(undoislem == 3){
                    copy_yenilentastoundo();
                    copy_redotoyenilen();
                    copy_matrix(board , undoboard, n );
                    copy_matrix(redoboard , board, n );
                    print_board(board,n);
                }
                else if(undoislem == 4){
                    copy_matrix(board , undoboard , n );
                    copy_yenilentastoundo();
                    printf("Hamle yapacaginiz yerin satir ve sutun numarasini giriniz2: ");
                    scanf("%d %d", &move_row, &move_col);
                    if (board[move_row-1][move_col-1] == ' ') {
                        board[move_row-1][move_col-1] = board[row-1][col-1] ;
                        board[row-1][col-1] = ' ';
                    if ( row == move_row ){
                        //burada bir tane dizinin icine atip sonrasinda oradakileri sayabilirim 
                        if(move_col- col == 2){
                            tasyeme( board[row-1][col] , player);
                            board[row-1][col] = ' ';
                        }
                        else if( move_col - col == -2 ){
                        tasyeme( board[row-1][col-2] , player);
                        board[row-1][col-2] = ' ';
                        }
                    }
                    else if ( col == move_col ){
                        if(move_row - row == 2){
                        tasyeme( board[row][col-1] , player );
                        board[row][col-1] = ' ';
                        }
                        else if(move_row - row == -2 ){
                        tasyeme( board[row-2][col-1] , player );
                        board[row-2][col-1] = ' ';
                        }
                    }
                    }
                    print_board(board,n);
                }
            }
            if(undoislem == 5 ){
                printf("\n\nOYUN SONA ERDI !!\n\n");
                // Oyuncuların puanlarını yazdırma
                int puan1 = puan_hesapla(yenilentas1);
                int puan2 = puan_hesapla(yenilentas2);
                printf("Oyuncu 1 Puanı: %d\n", puan1);
                printf("Oyuncu 2 Puanı: %d\n", puan2);
                determine_winner();
                print_yenilen_taslar();
                save_game(board,n);
                *pcikis1 = 1;
            }
        }
    }

}

// Oyunun bitip bitmedigini kontrol ettirme 
int isthisover(char **board , int n){
    int i,j , flag = 0 ;
    for(i = 0 ; i < n ; i++ ){
        for(j = 0 ; j < n ; j ++){
            if(isThisPiece(board[i][j])){
                if(n > i+2 && board[i+2][j] == ' '  && isThisPiece(board[i+1][j])){
                    flag = 1;
                }
                else if ((0 <= i-2 && board[i-2][j] == ' ' && isThisPiece(board[i-1][j]) )){
                    flag =1;
                }
                else if ((n > j+2 && board[i][j+2] == ' ' && isThisPiece(board[i][j+1]) )){
                    flag = 1;
                }
                else if ((0 <= j-2 && board[i][j-2] == ' ' && isThisPiece(board[i][j-1]))){
                    flag = 1;
                }
            }
        }
    }
    return flag;
}
//botun oynadigi tasi highlight etme 
void highlight_and_sleep(char **board, int n, int i, int j, int i2, int j2) {
    board[i][j] = board[i2][j2];
    board[i2][j2] = ' ';
    print_board(board, n);
    sleep(1);
}

//botu art arda  hamleler  icin cagiran fonskiyon 
void bot_artarda(char **board , int n, int player ,int i ,int j ){
        if((n > i+2 && board[i+2][j] == ' '  && isThisPiece(board[i+1][j])  )){
            highlight_and_sleep(board, n, i + 2, j, i, j);
            tasyeme(board[i+1][j],player);
            board[i+1][j] = ' ';
            print_board(board,n);
            bot_artarda(board,n,player,i+2,j);
        }
        else if ((0 <= i-2 && board[i-2][j] == ' ' && isThisPiece(board[i-1][j]) )){
            highlight_and_sleep(board, n, i - 2, j, i, j);
            tasyeme(board[i-1][j],player);
            board[i-1][j] = ' ';
            print_board(board,n);
            bot_artarda(board,n,player,i-2,j);

        }
        else if ((n > j+2 && board[i][j+2] == ' ' && isThisPiece(board[i][j+1]) )){
            highlight_and_sleep(board, n, i , j + 2, i, j );
            tasyeme(board[i][j+1],player);
            board[i][j+1] = ' ';
            print_board(board,n);
            bot_artarda(board,n,player,i,j+2);

        }
        else if ((0 <= j-2 && board[i][j-2] == ' ' && isThisPiece(board[i][j-1]))){
            highlight_and_sleep(board, n, i , j - 2, i, j );
            tasyeme(board[i][j-1],player);
            board[i][j-1] = ' ';
            print_board(board,n);
            bot_artarda(board,n,player,i,j-2);
        }
}

//Bot yazma denemeleri 
void bot_hamle(char **board , int n ,int player ){
    int i ,j , rakipmin = 10 , min = 10 , index1=0 ,index2=0, flag=0 ;
    int puan1 = puan_hesapla(yenilentas1);
    int puan2 = puan_hesapla(yenilentas2);
    
    for(i = 0 ; i < 5 ; i ++){
        if(rakipmin > (yenilentas1[i] - puan1)){
            rakipmin = (yenilentas1[i] - puan1);
            index1 = i;
        }
        if (min > (yenilentas2[i] - puan2)){
            min = (yenilentas2[i] - puan2);
            index2 = i;
        }
    }

    //Bot karsi oyuncuda buldugu max farka gore  karsidaki oyuncunun en az tasini alacak sekilde hamle yapiyor .
    if(rakipmin >= 0){   
        for( i = 0; i < n ;i++){
            for ( j = 0 ; j < n ; j++){
                if(board[i][j] == pieces [index1] ){
                    if( n > i+1 && 0 <= i-1 ){
                        if((board[i-1][j] == ' ' && flag ==0 && isThisPiece(board[i+1][j]) )){
                            highlight_and_sleep(board, n, i - 1, j, i + 1, j);
                            tasyeme(board[i][j],player);
                            board[i][j] = ' ';
                            flag=1;
                            bot_artarda(board,n,player,i-1,j);
                        }
                        else if ((board[i+1][j] == ' ' && flag == 0 && isThisPiece(board[i-1][j]))){
                            highlight_and_sleep(board, n, i + 1, j, i - 1, j);
                            tasyeme(board[i][j],player);
                            board[i][j] = ' ';
                            flag=1;
                            bot_artarda(board,n,player,i+1,j);
                        }
                    }
                    if( n > j+1 && 0 <= j-1 ){
                        if ((board[i][j-1] == ' ' && flag == 0 && isThisPiece(board[i][j+1]))){
                            highlight_and_sleep(board, n, i, j - 1, i, j + 1);
                            tasyeme(board[i][j],player);
                            board[i][j] =' ';
                            flag = 1 ;
                            bot_artarda(board,n,player,i,j-1);
                        }
                        else if ((board[i][j+1] == ' ' && flag == 0 && isThisPiece(board[i][j-1]))){
                            highlight_and_sleep(board, n, i, j + 1, i, j - 1);
                            tasyeme(board[i][j],player);
                            board[i][j] =' ';
                            flag = 1 ;
                            bot_artarda(board,n,player,i,j+1);
                        }
                    }

                }
            }
        }
    }
    //Eger rakibin yedigi taslar arasinda fark yoksa veya o tasi yiyemediyse kendisindeki en yuksek farka gore tas yemeye calisir.
    if (flag == 0 ) {
        for( i = 0; i < n ;i++){
            for ( j = 0 ; j < n ; j++){
                if(board[i][j] == pieces [index2] ){
                    if( n > i+1 && 0 <= i-1 ){
                        if((board[i-1][j] == ' ' && flag ==0 && isThisPiece(board[i+1][j]))){
                            highlight_and_sleep(board, n, i - 1, j, i + 1, j);
                            tasyeme(board[i][j],player);
                            board[i][j] = ' ';
                            flag=1;
                            bot_artarda(board,n,player,i-1,j);
                        }
                        else if ((board[i+1][j] == ' ' && flag==0 && isThisPiece(board[i-1][j]))){
                            highlight_and_sleep(board, n, i + 1, j, i - 1, j);
                            tasyeme(board[i][j],player);
                            board[i][j] = ' ';
                            flag=1;
                            bot_artarda(board,n,player,i+1,j);
                        }
                    }
                    if( n > j+1 && 0 <= j-1 ){
                        if ((board[i][j-1] == ' ' && flag == 0 && isThisPiece(board[i][j+1]))){
                            highlight_and_sleep(board, n, i, j - 1, i, j + 1);
                            tasyeme(board[i][j],player);
                            board[i][j] =' ';
                            flag = 1 ;
                            bot_artarda(board,n,player,i,j-1);
                        }
                        else if ((board[i][j+1] == ' ' && flag == 0 && isThisPiece(board[i][j-1]))){
                            highlight_and_sleep(board, n, i, j + 1, i, j - 1);
                            tasyeme(board[i][j],player);
                            board[i][j] =' ';
                            flag = 1 ;
                            bot_artarda(board,n,player,i,j+1);
                        }
                    }
                }
            }
        }
        //rakibin ihtiyaci tasi yiyemediyse veya kendisinin ihtiyaci tasida yiyemediyse boardi gezerek yapabilecegi ilk hamleyi yapar .
        if (flag == 0 ){
            for( i = 0 ; i < n ; i++){
                for (j = 0 ; j < n ; j++){
                    if(board[i][j] == ' ' && flag == 0 ){
                        if( i-2 >= 0 && isThisPiece(board[i-1][j]) && isThisPiece(board[i-2][j])){
                            highlight_and_sleep(board, n, i, j, i - 2, j);
                            tasyeme(board[i-1][j],player);
                            board[i-1][j] = ' ';
                            flag=1;
                            bot_artarda(board,n,player,i,j);
                        }
                    }
                    if(board[i][j] == ' ' && flag == 0 ){
                        if( i+2 < n && isThisPiece(board[i+1][j]) && isThisPiece(board[i+2][j])){
                            highlight_and_sleep(board, n, i, j, i + 2, j);
                            tasyeme(board[i+1][j],player);
                            board[i+1][j] = ' ';
                            flag=1;
                            bot_artarda(board,n,player,i,j);
                        }
                    }
                    if(board[i][j] == ' ' && flag == 0 ){
                        if(j-2 >= 0 && isThisPiece(board[i][j-1]) && isThisPiece(board[i][j-2])){
                            highlight_and_sleep(board, n, i, j, i, j - 2);
                            tasyeme(board[i][j-1],player);
                            board[i][j-1] = ' ';
                            flag=1;
                            bot_artarda(board,n,player,i,j);
                        }
                    }
                    if(board[i][j] == ' ' && flag == 0 ){
                        if(j+2 < n && isThisPiece(board[i][j+1]) && isThisPiece(board[i][j+2]) ){
                            highlight_and_sleep(board, n, i, j, i, j + 2);
                            tasyeme(board[i][j+1],player);
                            board[i][j+1] = ' ';
                            flag =1 ;
                            bot_artarda(board,n,player,i,j);
                        }
                    }
                }
            }
        }
    }
    print_board(board,n);
    
}

//main fonksiyonu
int main() {
    int n, i, j, mod ,cikis =0;
    int* pcikis = &cikis;
    // Rastgele sayı üreteciyi başlat
    srand(time(NULL));
// Oyunun başlangıç durumunu gösterme
    printf("\n!!!SKIPITTY OYUNUNA HOSGELDINIZ!!!\nOYUN BASLIYOR\n");
   
    printf("OYUNU HANGI MODDA OYNAMAK ISTERSINIZ ?\nMod 1 : 2 Oyuncu\nMod 2 : Bilgisayara karsi\nMod 3 : Kayitli oyunu yukle\n");
    scanf("%d",&mod); 

    // Kullanıcıdan matris boyutunu alma
    printf("Matris boyutunu girin (max 20): ");
    scanf("%d", &n);

    // Boyutun 20'den büyük olup olmadığını kontrol etme
    if (n > 20) {
        return 1;
    }

    // Dinamik bellek tahsisi
    char **board = (char **)malloc(n * sizeof(char *));
    char **undo_board = (char **)malloc(n * sizeof ( char *)) ;
    char **redo_board = (char **)malloc(n * sizeof(char *));
    for (i = 0; i < n; i++) {
        board[i] = (char *)malloc(n * sizeof(char));
        undo_board[i] = (char *)malloc(n * sizeof(char));
        redo_board[i] = (char *)malloc(n * sizeof(char));
    }
    
    
    if(mod == 1){
        // Taşları yerleştir
        place_pieces(board, n);
        // Oyuncuların sırayla hamle yapması
        print_board(board,n);
        int player = 1;  // 1: ilk oyuncu, 2: ikinci oyuncu
        while (cikis == 0 && isthisover(board ,n) ) {
            printf("\nOyuncu %d, sıra sizde:\n", player);
            player_move(board,undo_board,redo_board, n , player, pcikis);
        
            // Bir sonraki oyuncuya geçme
            player = (player == 1) ? 2 : 1;
        }
         printf("\n\nOYUN SONA ERDI !!\n\n");
            // Oyuncuların puanlarını yazdırma
            int puan1 = puan_hesapla(yenilentas1);
            int puan2 = puan_hesapla(yenilentas2);
            printf("Oyuncu 1 Puanı: %d\n", puan1);
            printf("Oyuncu 2 Puanı: %d\n", puan2);
            print_yenilen_taslar();
            determine_winner();
    }
    if (mod == 2 ){
        // Taşları yerleştir
        place_pieces(board, n);
        print_board(board,n);
        int player = 1;
        while(cikis == 0 && isthisover(board ,n)){
            if(player == 1 ){
                printf("\nOyuncu %d, sıra sizde:\n", player);
                player_move(board,undo_board,redo_board, n , player, pcikis);
            }
            else if(player == 2 ){
                printf("\n\nSira bilgisayarda:\n");
                bot_hamle(board,n,player);
            }  
            //Bir sonraki oyuncuya gecme
            player = (player == 1) ? 2 : 1;
        }
         printf("\n\nOYUN SONA ERDI !!\n\n");
            // Oyuncuların puanlarını yazdırma
            int puan1 = puan_hesapla(yenilentas1);
            int puan2 = puan_hesapla(yenilentas2);
            printf("Oyuncu 1 Puanı: %d\n", puan1);
            printf("Bilgisayar Puanı: %d\n", puan2);
            print_yenilen_taslar();
            determine_winner();

    }
    if ( mod == 3 ){
        load_game(&board,&n);
        print_board(board,n);
        int player = 1;  // 1: ilk oyuncu, 2: ikinci oyuncu
        while (cikis == 0 && isthisover(board ,n)) {
            printf("\nOyuncu %d, sıra sizde:\n", player);
            player_move(board,undo_board,redo_board, n , player, pcikis);
        
            // Bir sonraki oyuncuya geçme
            player = (player == 1) ? 2 : 1;
        }
         printf("\n\nOYUN SONA ERDI !!\n\n");
            // Oyuncuların puanlarını yazdırma
            int puan1 = puan_hesapla(yenilentas1);
            int puan2 = puan_hesapla(yenilentas2);
            printf("Oyuncu 1 Puanı: %d\n", puan1);
            printf("Oyuncu 2 Puanı: %d\n", puan2);
            print_yenilen_taslar();
            determine_winner();
    }

    // Dinamik belleği serbest bırakma
    for (i = 0; i < n; i++) {
        free(board[i]);
        free(undo_board[i]);
        free(redo_board[i]);
    }
    free(board);
    free(undo_board);
    free(redo_board);
    return 0;
}
//https://drive.google.com/drive/folders/16P3Un6nLEv2XcwQ9p_MjH4PRrwiyTts4
