	; BG샘플

	; INES 헤더
	.inesprg 1 ;   - 프로그램에 몇개의 뱅크를 사용하는가. 지금은 1개
	.ineschr 1 ;   - 그래픽 데이터에 몇개의 뱅크를 사용하는가. 지금은 1개.
	.inesmir 0 ;   - 수평 미러링
	.inesmap 0 ;   - 맵퍼. 0번으로 한다.

	.bank 1      ; 뱅크１
	.org $FFFA   ; $FFFA부터 개시

	.dw mainLoop ; VBlank 인터럽트 핸들러 (1/60초마다 mainLoop가 콜된다)
	.dw Start    ; 리셋 인터럽트. 리셋 인터럽트. 기동시와 리셋으로 Start로 점프
	.dw 0        ; 하드웨어 인터럽트와 소프트웨어 인터럽트에 의해 발생

	.bank 0			 ; 뱅크0
	.org $0000	 ; $0000부터 개시
Scroll_X:			 .db  0   ; X스크롤 값

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
	; PPU 컨트롤 레지스터 1 초기화
	lda #%00001000	; 여기서는 VBlank 인터럽트 금지
	sta $2000


waitVSync:
	lda $2002			; VBlank가 발생하면 $2002의 7비트째가 1이 된다
	bpl waitVSync  ; bit7이 0인 동안에는 waitVSync라벨의 위치로 점프하여 루프하며 계속 기다린다

	; PPU 컨트롤 레지스터 2 초기화
	lda #%00000110		; 초기화중에는 스프라이트와 BG를 표시 OFF로 한다
	sta $2001

    ;팔렛트를 로드
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

	; 속성(BG팔렛트 지정 데이터)을 로드

	; $23C0의 속성테이블에 로드한다
	lda #$23
	sta $2006
	lda #$C0
	sta $2006

	ldx #$00    ; X레지스터 클리어
	lda #%00000000				; 4개 모두 팔렛트 0번
	; 0번이나 1번으로 한다
loadAttrib
	eor #%01010101				; XOR연산으로 하나 건너의 비트를 교대로 0이나 1로 한다
	sta $2007							; $2007에 속성의 값($0이나 $55)을 읽어들임
	; 64회(전 캐릭터분)루프한다
	inx
	cpx #64
	bne loadAttrib

	; 네임테이블 생성

	; $2000의 네임테이블에 생성한다
	lda #$20
	sta $2006
	lda #$00
	sta $2006

	lda #$00        ; 0번(새까망)
	ldy #$00    ; Y레지스터 초기화
loadNametable1:
	ldx Star_Tbl, y			; Star테이블의 값을 X에 읽어들임
loadNametable2:
	sta $2007				; $2007에 속성의 값을 읽어들임
	dex							; X감산
	bne loadNametable2	; 아직 0이 아니라면 루프하여 검정을 출력한다
	; 1번이나 2번의 캐릭터를 Y의 값에서 번갈아 취득
	tya							; Y→A
	and #1					; A AND 1
	adc #1					; A에 1가산하여 1이나 2로
	sta $2007				; $2007에 속성의 값을 읽어들임
	lda #$00        ; 0번(새까망)
	iny							; Y가산
	cpy #20					; 20회(스타테이블의 수)루프한다
	bne loadNametable1

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

	; PPU 컨트롤 레지스터 1의 인터럽트 허가 플래그를 세운다
	lda #%10001000
	sta $2000

infinityLoop:					; VBlank 인터럽트 발생을 기다릴뿐인 무한루프
	jmp infinityLoop

mainLoop:					; 메인루프

	; 스프라이트 묘화(DMA를 이용)
	lda #$3  ; 스프라이트 데이터는 $0300번지부터이므로 3을 로드한다.
	sta $4014 ; 스프라이트 DMA레지스터에 A를 스토어하여 스프라이트 데이터를 DMA전송한다.
	
	; BG스크롤
	lda $2002			; 스크롤 값 클리어
	lda Scroll_X	; X의 스크롤 값을 로드
	sta $2005			; X방향 스크롤（Y방향은 고정)
	inc Scroll_X	; 스크롤 값을 가산

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

	rti									; 인터럽트로부터 복귀

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

	; 스타테이블 데이터(20개)
Star_Tbl    .db 60,45,35,60,90,65,45,20,90,10,30,40,65,25,65,35,50,35,40,35

tilepal: .incbin "giko2.pal" ; 팔렛트를 include한다

	.bank 2       ; 뱅크 2
	.org $0000    ; $0000부터 개시

	.incbin "giko2.bkg"  ; 배경 데이터의 바이너리 파일을 include한다
	.incbin "giko2.spr"  ; 스프라이트 데이터의 바이너리 파일을 include한다
