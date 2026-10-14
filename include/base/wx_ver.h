#ifndef WX_VERSION_H
#define WX_VERSION_H

/* Bump-up with each new version */
#define wxMAJOR_VERSION    1
#define wxMINOR_VERSION    6
#define wxRELEASE_NUMBER   8
#define wxVERSION_STRING "wxWindows 1.68"
#define wxVERSION_NUMBER (wxMAJOR_VERSION * 1000) + (wxMINOR_VERSION * 100) + wxRELEASE_NUMBER
#define wxBETA_NUMBER      2
#define wxVERSION_FLOAT float(wxMAJOR_VERSION + (wxMINOR_VERSION/10.0) + (wxRELEASE_NUMBER/100.0) + (wxBETA_NUMBER/10000.0))

#endif /* WX_VERSION_H */
