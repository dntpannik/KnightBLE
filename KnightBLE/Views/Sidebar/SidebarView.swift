//
//  SidebarView.swift
//  KnightBLE
//
//  Created by Christopher Janover on 1/8/23.
//

import SwiftUI

struct SidebarView: View {
    @EnvironmentObject var modelData: ModelData
    @Binding var isSidebarVisible: Bool
    
    let bgColor: Color =
        Color(.init(
            red: 52 / 255,
            green: 70 / 255,
            blue: 182 / 255,
            alpha: 1))
    
    let secondaryColor: Color =
        Color(.init(
            red: 100 / 255,
            green: 174 / 255,
            blue: 255 / 255,
            alpha: 1))
    
    var body: some View {
        ZStack {
            GeometryReader { _ in
                EmptyView()
            }
            .background(.black.opacity(0.6))
            .opacity(isSidebarVisible ? 1 : 0)
            .animation(.easeInOut.delay(0.2), value: isSidebarVisible)
            .onTapGesture {
                isSidebarVisible.toggle()
            }
            content
        }
        .edgesIgnoringSafeArea(.all)
        .frame(maxWidth: .infinity)
    }
        
    var content: some View {
        GeometryReader { geo in
            let sideBarWidth = geo.size.width * 0.7
            
            HStack(alignment: .top) {
                ZStack(alignment: .top) {
                    bgColor
                    MenuChevron
                    
                    VStack(alignment: .leading, spacing: 20) {
                        userProfile
                    }
                    .padding(.top, 80)
                    .padding(.horizontal, 40)
                }
                .frame(width: sideBarWidth)
                .offset(x: isSidebarVisible ? 0 : -sideBarWidth)
                .animation(.default, value: sideBarWidth)
                
                
                Spacer()
            }
        }
    }
    
    var userProfile: some View {
        VStack(alignment: .leading) {
            Button("Play Sounds") {
                modelData.PlaySounds()
                isSidebarVisible.toggle()
            }
            .buttonStyle(GrowingButton())
        }
    }
    
    var MenuChevron: some View {
        GeometryReader { geo in
            let sideBarWidth = geo.size.width-30
            ZStack {
                RoundedRectangle(cornerRadius: 18)
                    .fill(bgColor)
                    .frame(width: 60, height: 60)
                    .rotationEffect(Angle(degrees: 45))
                    .offset(x: isSidebarVisible ? -18 : -10)
                    .onTapGesture {
                        isSidebarVisible.toggle()
                    }
                
                Image(systemName: "chevron.right")
                    .foregroundColor(secondaryColor)
                    .rotationEffect(
                        isSidebarVisible ?
                        Angle(degrees: 180) : Angle(degrees: 0))
                    .offset(x: isSidebarVisible ? -4 : 8)
                    .foregroundColor(.blue)
            }
            .offset(x: sideBarWidth, y: 80)
            .animation(.default, value: isSidebarVisible)
        }
    }
}

struct SidebarView_Previews: PreviewProvider {
    static var previews: some View {
        SidebarView(isSidebarVisible: .constant(true))
            .environmentObject(ModelData(knights: [KnightWithAllAblities()]))
    }
}
