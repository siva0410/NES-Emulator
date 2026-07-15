	; 스프라이트 샘플 표시
	; INES 헤더
	.inesprg 1 ;   - 프로그램에 몇개의 뱅크를 사용하는 가. 지금은 1개.
	.ineschr 1 ;   - 그래픽 데이터에 몇개의 뱅크를 사용하는 가. 지금은 1개.
	.inesmir 0 ;   - 수평미러링
	.inesmap 0 ;   - 맵퍼. 0번으로 한다

	.bank 1      ; 뱅크 1
	.org $FFFA   ; $FFFA부터 개시

	.dw 0        ; VBlank 인터럽트
	.dw Start    ; 리셋 인터럽트. 기동시와 리셋으로 Start로 점프
	.dw 0        ; 하드웨어 인터럽트와 소프트웨어 인터럽트에 의해 발생

	.bank 0			; 뱅크０
	.org $0000  ; $0000부터 개시
X_Pos   .db 0	 ; 스프라이트 X좌표의 변수($0000)
Y_Pos   .db 0	 ; 스프라이트 Y좌표의 변수($0001)

	.org $8000   ; $8000부터 개시
Start:  
	lda $2002  ; VBlank가 발생하면 $2002의 7비트째가 1이 된다
	bpl Start  ; bit7이 0인 동안에는 Start라벨의 위치로 점프하여 루프하여 기다림

	; PPU컨트롤 레지스터 초기화
	lda #%00001000 
	sta $2000
	lda #%00000110		; 초기화중에는 스프라이트와 BG를 표시OFF로 한다
	sta $2001

	ldx #$00    ; X레지스터 클리어

	; VRAM어드레스 레지스터의 $2006에 팔렛트의 로드처의 어드레스 $3F00을 지정한다.
	lda #$3F      ; have $2006 tell
	sta $2006     ; $2007 to start
	lda #$00      ; at $3F00 (pallete).
	sta $2006

loadPal: ; 라벨은 [라벨명+:]의 형식으로 기술
	lda tilepal, x ; A에 (ourpal + x)번지의 팔렛트를 로드한다

	sta $2007 ; $2007에 팔렛트의 값을 읽어들임

	inx ; X레지스터에 값을 1 가산하고 있다

	cpx #32 ; X를 32(10진수.BG와 스프라이트의 팔렛트의 총수)와 비교하여 같은지 어떤지를 비교하고 있다	
	bne loadPal ;	위가 같지 않은 경우는 loadpal라벨의 위치로 점프한다
	; X가 32라면 팔렛트 로드 종료

	; 스프라이트 좌표 초기화
	lda X_Pos_Init
	sta X_Pos
	lda Y_Pos_Init
	sta Y_Pos

	; PPU 콘트롤 레지스터 2 초기화
	lda #%00011110	; 스프라이트와 BG의 표시를 ON으로 한다
	sta $2001

mainLoop:					; 메인루프
	lda $2002  ; VBlank가 발생하면 $2002의 7비트째가 1이 됩니다.
	bpl mainLoop  ; bit7이 0인 동안에는 mainLoop라벨의 위치로 점프해 루프하여 계속 기다립니다.
	
	; 스프라이트 묘화
	lda #$00   ; $00(스프라이트 RAM의 어드레스는 8비트 길이)을 A에 로드
	sta $2003  ; A의 스프라이트 RAM의 어드레스를 스토어

	lda Y_Pos   ; Y좌표의 값을 로드
	sta $2004   ; Y좌표를 레지스터에 스토어한다.
	
	lda #00     ; 0(10진수)을 A에 로드
	sta $2004   ; 0을 스토어해서 0번의 스프라이트를 지정한다
	sta $2004   ; 반전이나 우선순위는 조작하지 않으므로 재차 $00을 스토어한다
	
	lda X_Pos	; X좌표의 값을 로드
	sta $2004   ; X좌표를 레지스터에 스토어한다

	; 패드 I/O의 레지스터의 준비
	lda #$01
	sta $4016
	lda #$00 
	sta $4016

	; 패드 입력 체크
	lda $4016  ; A버튼을 스킵
	lda $4016  ; B버튼을 스킵
	lda $4016  ; Select버튼을 스킵
	lda $4016  ; Start버튼을 스킵
	lda $4016  ; UP버튼
	and #1     ; AND #1
	bne UPKEYdown  ; 0이 아니라면 눌렸으므로 UPKeydown으로 점프
	
	lda $4016  ; DOWN버튼
	and #1     ; AND #1
	bne DOWNKEYdown ; 0이 아니라면 눌렸으므로 DOWNKeydown으로 점프

	lda $4016  ; LEFT버튼
	and #1     ; AND #1
	bne LEFTKEYdown ; 0이 아니라면 눌렸으므로 LEFTKeydown으로 점프

	lda $4016  ; RIGHT버튼
	and #1     ; AND #1
	bne RIGHTKEYdown ; 0이 아니라면 눌렸으므로 RIGHTKeydown으로 점프
	jmp NOTHINGdown  ; 아무것도 눌려지지 않았다면 NOTHINGdown으로

UPKEYdown:
	dec Y_Pos	; Y좌표를 1 감산. 제로페이지이므로 아래의 코드를 이 1명령으로 단축할 수 있다
;	lda Y_Pos ; Y좌표를 로드
;	sbc #1  ; #1감산한다
;	sta Y_Pos ; Y좌표를 스토어

	jmp NOTHINGdown

DOWNKEYdown:
	inc Y_Pos ; Y좌표를 1가산
	jmp NOTHINGdown

LEFTKEYdown:
	dec X_Pos	; X좌표를 1감산
	jmp NOTHINGdown 

RIGHTKEYdown:
	inc X_Pos	; X좌표를 1가산
	; 이 다음 NOTHINGdown이므로 점프할 필요 없음

NOTHINGdown:
	jmp mainLoop				; mainLoop의 최초로 돌아간다

	; 초기 데이터
X_Pos_Init   .db 20       ; X좌표 초기값
Y_Pos_Init   .db 40       ; Y좌표 초기값

tilepal: .incbin "giko.pal" ; 팔렛트를 include한다

	.bank 2       ; 뱅크 2
	.org $0000    ; $0000부터 개시

	.incbin "giko.bkg"  ; 배경데이터의 바이너리 파일을 include한다
	.incbin "giko.spr"  ; 스프라이트데이터의 바이너리 파일을 include한다
