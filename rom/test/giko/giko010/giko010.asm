	; 서브루틴 샘플

	; INES 헤더
	.inesprg 1 ;   - 프로그램에 몇개의 뱅크를 사용하는가. 지금은 1개
	.ineschr 1 ;   - 그래픽 데이터에 몇개의 뱅크를 사용하는가. 지금은 1개.
	.inesmir 0 ;   - 수평 미러링
	.inesmap 0 ;   - 맵퍼. 0번으로 한다.

	.bank 1      ; 뱅크1
	.org $FFFA   ; $FFFA부터 개시

	.dw 0        ; VBlank 인터럽트
	.dw Start    ; 리셋 인터럽트. 리셋 인터럽트. 기동시와 리셋으로 Start로 점프
	.dw 0        ; 하드웨어 인터럽트와 소프트웨어 인터럽트에 의해 발생

	.bank 0			 ; 뱅크0
	.org $0300	 ; $0300부터 개시, 스프라이트 DMA 데이터 배치
Sprite1_Y:     .db  0   ; 스프라이트 #1 Y좌표
Sprite1_T:     .db  0   ; 스프라이트 #1 넘버
Sprite1_S:     .db  0   ; 스프라이트 #1 속성
Sprite1_X:     .db  0   ; 스프라이트 #1 X좌표
Sprite2_Y:     .db  0   ; 스프라이트 #2 Y좌표
Sprite2_T:     .db  0   ; 스프라이트 #2 넘버
Sprite2_S:     .db  0   ; 스프라이트 #2 속성
Sprite2_X:     .db  0   ; 스프라이트 #2 X좌표

	.org $8000	 ; $8000부터 개시
Start:
	lda $2002  ; VBlank가 발생하면 $2002의 7비트째가 1이 된다
	bpl Start  ; bit7이 0인 동안에는 Start라벨의 위치로 점프하여 루프하며 기다림

	; PPU컨트롤 레지스터 초기화
	lda #%00001000
	sta $2000
	lda #%00000110		; 초기화중에는 스프라이트와 BG를 표시OFF로 한다
	sta $2001

	ldx #$00    ; X레지스터 클리어

	; VRAM어드레스 레지스터인 $2006에 팔렛트의 로드처의 어드레스 $3F00을 지정한다.
	lda #$3F    ; have $2006 tell
	sta $2006   ; $2007 to start
	lda #$00    ; at $3F00 (pallete).
	sta $2006

loadPal: ; 라벨은 [라벨명+:]의 형식으로 기술
	lda tilepal, x ; A에 (ourpal + x)번지의 팔렛트를 로드한다

	sta $2007 ; $2007에 팔렛트의 값을 읽어들임

	inx ; X레지스터에 값을 1 가산하고 있다

	cpx #32 ; X를 32(10진수.BG와 스프라이트의 팔렛트의 총수)와 비교하여 같은지 어떤지를 비교하고 있다	
	bne loadPal ;	위가 같지 않은 경우는 loadpal라벨의 위치로 점프한다
	; X가 32라면 팔렛트 로드 종료

	; １번째 스프라이트 좌표 초기화
	lda X_Pos_Init
	sta Sprite1_X
	lda Y_Pos_Init
	sta Sprite1_Y
	; ２번째 스프라이트 좌표 갱신 서브루틴을 콜
	jsr setSprite2
	; ２번째 스프라이트를 수평반전
	lda #%01000000
	sta Sprite2_S

	; PPU 컨트롤 레지스터 2초기화
	lda #%00011110	; 스프라이트와 BG의 표시를 ON으로 한다
	sta $2001

mainLoop:					; 메인루프
	lda $2002  ; VBlank가 발생하면 $2002의 7비트째가 1이 됩니다.
	bpl mainLoop  ; bit7이 0인 동안에는 mainLoop라벨의 위치로 점프해 루프하여 계속 기다립니다.

	; 스프라이트 묘화(DMA를 이용)
	lda #$3  ; 스프라이트 데이터는 $0300번지부터이므로 3을 로드한다.
	sta $4014 ; 스프라이트 DMA레지스터에 A를 스토어하여 스프라이트 데이터를 DMA전송한다.
	
	; 패드 I/O 레지스터의 준비
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
	dec Sprite1_Y	; Y좌표를 1감산
	jmp NOTHINGdown

DOWNKEYdown:
	inc Sprite1_Y ; Y좌표를 1가산
	jmp NOTHINGdown

LEFTKEYdown:
	dec Sprite1_X	; X좌표를 1감산
	jmp NOTHINGdown 

RIGHTKEYdown:
	inc Sprite1_X	; X좌표를 1가산
	; 이 다음NOTHINGdown이므로 점프할 필요없음

NOTHINGdown:
	; 2번째 스프라이트 좌표갱신 서브루틴을 콜
	jsr setSprite2

	jmp mainLoop				; mainLoop의 처음으로 돌아감

setSprite2:
	; 2번째 스프라이트의 좌표갱신 서브루틴
	lda Sprite1_X
	adc #8 		; 8도트 오른쪽으로 물린다
	sta Sprite2_X
	lda Sprite1_Y
	sta Sprite2_Y
	rts

	; 초기 데이터
X_Pos_Init   .db 20       ; X좌표 초기값
Y_Pos_Init   .db 40       ; Y좌표 초기값

tilepal: .incbin "giko2.pal" ; 팔렛트를 include한다

	.bank 2       ; 뱅크 2
	.org $0000    ; $0000부터 개시

	.incbin "giko.bkg"  ; 배경 데이터의 바이너리 파일을 include한다
	.incbin "giko2.spr"  ; 스프라이트 데이터의 바이너리 파일을 include한다
