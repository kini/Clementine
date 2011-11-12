/* This file is part of Clementine.
   Copyright 2010, David Sansome <me@davidsansome.com>

   Clementine is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Clementine is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Clementine.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "playlistheader.h"
#include "playlistview.h"

#include <QtDebug>
#include <QContextMenuEvent>
#include <QMenu>
#include <QSignalMapper>

PlaylistHeader::PlaylistHeader(Qt::Orientation orientation, QWidget* parent)
    : StretchHeaderView(orientation, parent),
      menu_(new QMenu(this)),
      show_mapper_(new QSignalMapper(this))
{
  hide_action_ = menu_->addAction(tr("&Hide..."), this, SLOT(HideCurrent()));
  stretch_action_ = menu_->addAction(tr("&Stretch columns to fit window"), this, SLOT(ToggleStretchEnabled()));
  menu_->addSeparator();

  QMenu* align_menu = new QMenu(tr("&Align text"), this); align_left_action_ = align_menu->addAction(tr("&Left"), this, SLOT(AlignCurrentLeft()));
  align_center_action_ = align_menu->addAction(tr("&Center"), this, SLOT(AlignCurrentCenter()));
  align_right_action_ = align_menu->addAction(tr("&Right"), this, SLOT(AlignCurrentRight()));

  menu_->addMenu(align_menu);
  menu_->addSeparator();

  stretch_action_->setCheckable(true);
  stretch_action_->setChecked(is_stretch_enabled());

  connect(show_mapper_, SIGNAL(mapped(int)), SLOT(ToggleVisible(int)));
  connect(this, SIGNAL(StretchEnabledChanged(bool)), stretch_action_, SLOT(setChecked(bool)));
}

void PlaylistHeader::contextMenuEvent(QContextMenuEvent* e) {
  menu_section_ = logicalIndexAt(e->pos());

  if (menu_section_ == -1 || (
        menu_section_ == logicalIndex(0) && logicalIndex(1) == -1))
    hide_action_->setVisible(false);
  else {
    hide_action_->setVisible(true);

    QString title(model()->headerData(menu_section_, Qt::Horizontal).toString());
    hide_action_->setText(tr("&Hide %1").arg(title));
  }

  qDeleteAll(show_actions_);
  show_actions_.clear();
  for (int i=0 ; i<count() ; ++i) {
    AddColumnAction(i);
  }

  menu_->popup(e->globalPos());
}

void PlaylistHeader::AddColumnAction(int index) {
  QString title(model()->headerData(index, Qt::Horizontal).toString());

  QAction* action = menu_->addAction(title, show_mapper_, SLOT(map()));
  action->setCheckable(true);
  action->setChecked(!isSectionHidden(index));
  show_actions_ << action;

  show_mapper_->setMapping(action, index);
}

void PlaylistHeader::HideCurrent() {
  if (menu_section_ == -1)
    return;

  SetSectionHidden(menu_section_, true);
}

void PlaylistHeader::AlignCurrentLeft() {
  static_cast<PlaylistView*>(parent())->SetColumnAlignment(
        menu_section_, Qt::AlignLeft | Qt::AlignVCenter);
}

void PlaylistHeader::AlignCurrentCenter() {
  static_cast<PlaylistView*>(parent())->SetColumnAlignment(
        menu_section_, Qt::AlignHCenter | Qt::AlignVCenter);
}

void PlaylistHeader::AlignCurrentRight() {
  static_cast<PlaylistView*>(parent())->SetColumnAlignment(
        menu_section_, Qt::AlignRight | Qt::AlignVCenter);
}

void PlaylistHeader::ToggleVisible(int section) {
  SetSectionHidden(section, !isSectionHidden(section));
  emit SectionVisibilityChanged(section, !isSectionHidden(section));
}

void PlaylistHeader::enterEvent(QEvent*) {
  emit MouseEntered();
}
