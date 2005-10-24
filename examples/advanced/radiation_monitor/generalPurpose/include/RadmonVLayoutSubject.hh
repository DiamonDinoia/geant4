//
// File name:     RadmonVLayoutSubject.hh
// Creation date: Sep 2005
// Main author:   Riccardo Capra <capra@ge.infn.it>
//
// Id:            $Id: RadmonVLayoutSubject.hh,v 1.1 2005-10-24 14:51:36 capra Exp $
// Tag:           $Name: not supported by cvs2svn $
//
// Description:   Subject class of the observer-subjectmodel for the detector
//                layout
//

#ifndef   RADMONVLAYOUTSUBJECT_HH
 #define  RADMONVLAYOUTSUBJECT_HH
 
 // Include files
 #include <set>
 
 // Forward declarations
 class RadmonVLayoutObserver;
 
 class RadmonVLayoutSubject
 {
  public:
   void                                         AttachObserver(RadmonVLayoutObserver * observer);
   void                                         DetachObserver(RadmonVLayoutObserver * observer);

  protected:
   inline                                       RadmonVLayoutSubject();
   inline                                      ~RadmonVLayoutSubject();

   void                                         NotifyChange(void);

  private:
  // Hidden constructors and operators
                                                RadmonVLayoutSubject(const RadmonVLayoutSubject & copy);
   RadmonVLayoutSubject &                       operator=(const RadmonVLayoutSubject & copy);

  // Private attributes
   typedef std::set<RadmonVLayoutObserver *> ObserversSet;
   ObserversSet                                 observersSet;
 };

 // Inline implementations
 #include "RadmonVLayoutSubject.icc"
#endif /* RADMONVLAYOUTSUBJECT_HH */
