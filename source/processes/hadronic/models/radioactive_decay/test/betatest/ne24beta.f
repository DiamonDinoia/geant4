*    
*     ********************************************************************
*     * DISCLAIMER                                                       *
*     *                                                                  *
*     * The following disclaimer summarizes all the specific disclaimers *
*     * of contributors to this software. The specific disclaimers,which *
*     * govern, are listed with their locations in:                      *
*     *   http://cern.ch/geant4/license                                  *
*     *                                                                  *
*     * Neither the authors of this software system, nor their employing *
*     * institutes,nor the agencies providing financial support for this *
*     * work  make  any representation or  warranty, express or implied, *
*     * regarding  this  software system or assume any liability for its *
*     * use.                                                             *
*     *                                                                  *
*     * This  code  implementation is the  intellectual property  of the *
*     * GEANT4 collaboration.                                            *
*     * By copying,  distributing  or modifying the Program (or any work *
*     * based  on  the Program)  you indicate  your  acceptance of  this *
*     * statement, and all its terms.                                    *
*     ********************************************************************
*    
      program testbeta

      parameter (nwpawc = 2000000,mtuple=20)
      common/PAWC/PAW(nwpawc)
      
      real  spec(100)
      real spec1(100)
      
      integer Ntotal, A, Z
      real Q
      character*80 filename
      character*1 mode
      do i = 1,100
         spec(i) = 0.
         spec1(i) = 0.
      enddo

c
      call hlimit(nwpawc)
      call hropen(1,'EXAMPLE','ne24.hbook','N',1024,istat)
      if (istat.ne.0) go to 99
      call hbook1(100,'beta-theory ',100,0.,2000.,0.)
      call hbook1(200,'beta-Geant4 ',100,0.,2000.,0.)
      call hbook1(300,'decay profile', 100, 0.,3000.,0.) 
      sum1 = 0.
      sum2 = 0.
      ee = .511
      Q1 = 1.9978
      Q2 = 1.1234
      zd = 11
      dx = 2./100
      e1 = q1/ee + 1.
      e2 = q2/ee + 1.
c
      do i=1,100
         X=i*dx
         G=X/ee+ 1.0
         F=F2(G,zd)
c         print*, G,F
         spec(i)=F*SQRT(G*G-1.0)*(E1-G)*(E1-G)*G
         sum1 = sum1 + spec(i)
	 if (x .le. q2) then 
            spec1(i)=F*SQRT(G*G-1.0)*(E2-G)*(E2-G)*G
            sum2 = sum2 + spec1(i)
         endif
      enddo
      do i = 1,100
         spec(i) = spec(i) /sum1*0.92 + spec1(i) /sum2*0.08
         spec1(i) = 0.
      enddo
c      print *, ' '
c      print *, " Name of the data file"
c      read(*,*) filename
      open(11, file='ne24.data', status='old')
      do while (.true.)
         read(11,*, end=30) e, x, x
         k = e/20. + 1
         spec1(k) = spec1(k) +1.
         call hfill (300, x, 0.,1.)
      enddo
 30   sum1 = 0.
      do i = 1,100
         sum1 = sum1 +spec1(i)
      enddo          
      do i = 1,100
         spec1(i) = spec1(i) /sum1
      enddo
      
      
      
      call hpak(100,spec)
      call hpak(200,spec1)
      
      call hrout(0,ICYCLE,' ')
c     
      call hrend('EXAMPLE')       
      
*     
 99   continue
      
      
      end
      
      FUNCTION F2(X,z0)
      PI=4.0*ATAN(1.0)
      P=SQRT(X*X-1.0) 
      U=Z0/137.0
      S=SQRT(1.0-U*U) - 1.
      Y = 2*PI*U*X/P
      A1 = U*U*X*X + P*P/4.
      A2 = abs(Y/(1-exp(-Y)))
      F2=A1**S * A2
      RETURN
      END









