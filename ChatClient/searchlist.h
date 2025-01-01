#ifndef SEARCHLIST_H
#define SEARCHLIST_H

#include <QListWidget>

class SearchInfo;
class SearchList : public QListWidget
{
    Q_OBJECT
public:
    SearchList(QWidget *parent = nullptr);
    void CloseFindDlg();
    void SetSearchEdit(QWidget* edit);

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void initUI();
    void initSignals();
    void waitPending(bool pending = true);
    void initTipItem();

private:
    bool _send_pending;
    QWidget* _search_edit;
    // LoadingDlg * _loadingDialog;
    std::shared_ptr<QDialog> _find_dlg;

private slots:
    void slot_item_clicked(QListWidgetItem *item);
    void slot_user_search(std::shared_ptr<SearchInfo> si);
};

#endif // SEARCHLIST_H
