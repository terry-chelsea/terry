/*
 * =====================================================================================
 *
 *       Filename:  subject.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/13/13 22:02:33
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  huyao (H.Y), yaoshunyuhuyao@163.com
 *        Company:  NDSL
 *
 * =====================================================================================
 */

#include <string>

using namespace std;
class Observer;

class Subject
{
    public : 
        Subject(string name) ;

        ~Subject();
        void cry(string name);
        void laugh(string name);

        void stop_cry();
        void stop_laugh();

        void join_laugh(Observer *);
        void quit_laugh();
        void join_cry(Observer *);
        void quit_cry();

        void start_laugh();
        void start_cry();

        string get_name()
        {
            return m_name;
        }

    private :
        string m_name;
        Observer *m_laugh;
        Observer *m_cry;
};
